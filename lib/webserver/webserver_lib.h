#ifndef SUNDA_WEBSERVER_LIB_H
#define SUNDA_WEBSERVER_LIB_H

#include "../../core/lang/interpreter.h"
#include "tcp_server.h"
#include "http_parser.h"
#include "../json/json_lib.h"
#include <map>
#include <functional>
#include <atomic>

extern std::atomic<bool> g_interrupt;

namespace WebServer {

struct Route {
    std::string method;
    std::string path;
    Value handler; // Sunda closure
};

class ServerInstance {
public:
    std::vector<Route> routes;
    TCPServer server;
    // Middleware
    std::vector<Value> middlewares;
    // Grouping
    std::string currentPrefix = "";

    void add_route(std::string method, std::string path, Value handler) {
        std::string fullPath = currentPrefix + path;
        // Fix double slashes slightly
        if (currentPrefix.length() > 0 && currentPrefix.back() == '/' && path.length() > 0 && path[0] == '/') {
            fullPath = currentPrefix + path.substr(1);
        }
        routes.push_back({method, fullPath, handler});
    }

    void use(Value middleware) {
        middlewares.push_back(middleware);
    }

    void listen(int port, Interpreter& interpreter, std::string cert = "", std::string key = "") {
        std::string finalCert = cert;
        std::string finalKey = key;

        std::string scriptDir = "";
        size_t lastSlash = interpreter.currentFile.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            scriptDir = interpreter.currentFile.substr(0, lastSlash + 1);
        }

        if (!cert.empty() && cert[0] != '/' && !scriptDir.empty()) {
            finalCert = scriptDir + cert;
        }
        if (!key.empty() && key[0] != '/' && !scriptDir.empty()) {
            finalKey = scriptDir + key;
        }

        if (!finalCert.empty() && !finalKey.empty()) {
             if (!server.initSSL(finalCert, finalKey)) {
                 std::cerr << "Failed to init SSL (files not found or invalid: " << finalCert << ", " << finalKey << "). Falling back to HTTP." << std::endl;
             }
        }
        
        if (!server.start(port)) {
            std::cerr << "Failed to start server on port " << port << std::endl;
            return;
        }

        while (!g_interrupt) {
            TCPServer::Client client = server.accept_connection();
            if (client.fd < 0) continue; 

            std::string raw_req = server.read_request(client);
            if (raw_req.empty()) {
                 server.close_client(client);
                 continue;
            }

            HttpRequest req = HTTPParser::parse(raw_req);
            
            // Shared finish flag
            auto handled = std::make_shared<bool>(false);

            // Logic to run Router (last middleware)
            std::function<void()> runRouter = [&, req, client, handled]() {
                 bool found = false;
                 for (auto& route : routes) {
                    if (route.method == req.method) {
                        std::map<std::string, std::string> params;
                        if (HTTPParser::match_route(route.path, req.path, params)) {
                            Value ctx = create_context(req, params, client, handled);
                            std::vector<Value> args = { ctx };
                            Value result = interpreter.callClosure(route.handler, args);
                            if (!*handled) {
                                server.send_response(client, result.toString());
                                *handled = true;
                            }
                            found = true;
                            break;
                        }
                    }
                 }
                 if (!found && !*handled) {
                     std::string body = "404 Not Found";
                     std::string res = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;
                     server.send_response(client, res);
                     *handled = true;
                 }
            };

            // Middleware Dispatch Chain
            // Logic: dispatch(0) calls mw[0]. mw[0] receives next() which calls dispatch(1).
            // If mw[0] doesn't call next(), chain stops.
            // If i == size, call runRouter.
            
            // Note: std::function definition needed for recursion
            std::function<void(size_t)> dispatch;
            dispatch = [&](size_t i) {
                if (*handled) return;
                if (i >= middlewares.size()) {
                    runRouter();
                    return;
                }
                
                std::map<std::string, std::string> no_params;
                Value ctx = create_context(req, no_params, client, handled);
                
                // Add next() function
                // Requires direct access to mapVal shared_ptr
                if (ctx.isMap && ctx.mapVal) {
                     (*ctx.mapVal)["next"] = Value([&, i](std::vector<Value> args) -> Value {
                         dispatch(i + 1);
                         return Value("", 0, false);
                     });
                }

                std::vector<Value> args = { ctx };
                try {
                    interpreter.callClosure(middlewares[i], args);
                } catch (const std::exception& e) {
                    std::cerr << "Middleware error: " << e.what() << std::endl;
                    if (!*handled) {
                        std::string res = "HTTP/1.1 500 Internal Server Error\r\n\r\nMiddleware Error";
                        server.send_response(client, res);
                        *handled = true;
                    }
                }
            };

            dispatch(0);
            server.close_client(client);
        }
    }

    Value create_context(HttpRequest req, std::map<std::string, std::string> params, TCPServer::Client client, std::shared_ptr<bool> handled) {
        std::map<std::string, Value> ctx_map;
        std::map<std::string, Value> req_map;
        req_map["path"] = Value(req.path, 0, false);
        req_map["method"] = Value(req.method, 0, false);
        req_map["body"] = Value(req.body, 0, false);
        req_map["param"] = Value([params](std::vector<Value> args) -> Value {
            if (args.empty()) return Value("undefined", 0, false);
            std::string p = args[0].strVal;
            if (params.count(p)) return Value(params.at(p), 0, false);
             return Value("undefined", 0, false);
        });
        
        // Context.req.params object
        std::map<std::string, Value> params_obj;
        for (auto const& [key, val] : params) {
            params_obj[key] = Value(val, 0, false);
        }
        req_map["params"] = Value(params_obj);

        req_map["header"] = Value([req](std::vector<Value> args) -> Value {
             if (args.empty()) return Value("undefined", 0, false);
             std::string h = args[0].strVal;
             // Headers are typically case-insensitive but for simplicity direct match
             if (req.headers.count(h)) return Value(req.headers.at(h), 0, false);
              return Value("undefined", 0, false);
        });
        req_map["json"] = Value([req](std::vector<Value> args) -> Value {
             JSONLib::JsonParser parser(req.body);
             return parser.parse();
        });
        ctx_map["req"] = Value(req_map);

        auto send_res = [this, client, handled](const std::string& res) {
            if (!*handled) {
                server.send_response(client, res);
                *handled = true;
            }
        };

        ctx_map["text"] = Value([send_res](std::vector<Value> args) -> Value {
            std::string body = args.empty() ? "" : args[0].toString();
            std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;
            send_res(res);
            return Value(res, 0, false); // For chaining if needed, but mainly side-effect
        });
        ctx_map["json"] = Value([send_res](std::vector<Value> args) -> Value {
             std::string body = args.empty() ? "{}" : args[0].toJson();
             std::string res = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;
             send_res(res);
             return Value(res, 0, false);
        });
        ctx_map["html"] = Value([send_res](std::vector<Value> args) -> Value {
             std::string body = args.empty() ? "" : args[0].toString();
             std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n" + body;
             send_res(res);
             return Value(res, 0, false);
        });
        ctx_map["status"] = Value([ctx_map](std::vector<Value> args) -> Value {
             // Mock status chaining
             return Value(ctx_map);
        });

        return Value(ctx_map);
    }
};

static std::vector<std::shared_ptr<ServerInstance>> g_servers;

void register_webserver(Interpreter& interpreter) {
    static Interpreter* s_interpreter = &interpreter; 
    
    interpreter.registerNative("Webserver", [](std::vector<Value> args) -> Value {
        auto instance = std::make_shared<ServerInstance>();
        g_servers.push_back(instance);
        
        std::map<std::string, Value> server_obj;
        
        server_obj["use"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.empty()) return Value("", 0, false);
            instance->use(args[0]);
            return Value("", 1, true);
        });

        // Group Implementation
        server_obj["group"] = Value([instance](std::vector<Value> args) -> Value {
             if (args.size() < 2) return Value("", 0, false);
             std::string prefix = args[0].strVal;
             Value callback = args[1];

             std::string oldPrefix = instance->currentPrefix;
             instance->currentPrefix += prefix;
             
             // Run callback
             std::vector<Value> cbArgs;
             s_interpreter->callClosure(callback, cbArgs);

             instance->currentPrefix = oldPrefix;
             return Value("", 1, true); 
        });

        server_obj["get"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.size() < 2) return Value("", 0, false);
            instance->add_route("GET", args[0].strVal, args[1]);
            return Value("", 1, true); 
        });

        server_obj["post"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.size() < 2) return Value("", 0, false);
            instance->add_route("POST", args[0].strVal, args[1]);
            return Value("", 1, true);
        });

        server_obj["put"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.size() < 2) return Value("", 0, false);
            instance->add_route("PUT", args[0].strVal, args[1]);
            return Value("", 1, true);
        });

        server_obj["delete"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.size() < 2) return Value("", 0, false);
            instance->add_route("DELETE", args[0].strVal, args[1]);
            return Value("", 1, true);
        });

        server_obj["patch"] = Value([instance](std::vector<Value> args) -> Value {
            if (args.size() < 2) return Value("", 0, false);
            instance->add_route("PATCH", args[0].strVal, args[1]);
            return Value("", 1, true);
        });

        server_obj["listen"] = Value([instance](std::vector<Value> args) -> Value {
            int port = 3000;
            std::string cert = "";
            std::string key = "";
            
            if (!args.empty() && args[0].isMap) {
                auto m = args[0].mapVal;
                if (m->count("port") && (*m)["port"].isInt) port = (*m)["port"].intVal;
                if (m->count("cert") && (*m)["cert"].isInt == false) cert = (*m)["cert"].strVal;
                if (m->count("key") && (*m)["key"].isInt == false) key = (*m)["key"].strVal;
            }
            instance->listen(port, *s_interpreter, cert, key);
            return Value("", 0, false);
        });

        return Value(server_obj);
    });
}

} // namespace WebServer

#endif
