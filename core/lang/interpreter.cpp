#include "interpreter.h"
#include <iostream>

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    
    // Default natives
    registerNative("print", [](std::vector<Value> args) {
        for(auto& a : args) std::cout << a.toString();
        return Value("", 0, true);
    });
    registerNative("println", [](std::vector<Value> args) {
        for(auto& a : args) std::cout << a.toString();
        std::cout << std::endl;
        return Value("", 0, true);
    });
}

void Interpreter::setVar(std::string name, Value v) {
    // If not found in chain, define in globals? 
    // Or closer scope?
    // Interpreter Logic: assign if exists, else define in current?
    // User requested behavior: "var count" defines it. "count = 1" assigns.
    // If we just use setVar for both?
    // Current Sunda: `varDecl` uses define. `assignment` uses assign.
    
    // Helper: this function is for assignment (update)
    environment->assign(name, v);
    // If assign failed (env didn't find it), what happens?
    // My Environment::assign does recursion. If root doesn't have it?
    // It does nothing.
    // We should probably check or callback error.
    // However, for MVP, we might want implicit global definition if missing?
    // Let's stick to explicit `define` in VarDecl.
}

Value Interpreter::getVar(std::string name) {
    return environment->get(name);
}

void Interpreter::registerNative(std::string name, std::function<Value(std::vector<Value>)> func) {
    natives[name] = func;
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    for (auto& s : statements) {
        if (s) execute(s);
    }
}

// Forward declaration of render_gui bridging helper if needed?
#include "interpreter.h"
#include "lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    if (isReturning) return;

    if (auto imp = std::dynamic_pointer_cast<ImportStmt>(stmt)) {
        // JIT Loading
        // std::cout << "[JIT] Loading module: " << imp->moduleName << std::endl;
        
        if (imp->moduleName == "gui" || imp->moduleName == "math") {
            return;
        }

        // File loading
        std::string filename = imp->moduleName;
        if (filename.length() < 2 || filename.substr(filename.length()-2) != ".s") filename += ".s";
        
        std::ifstream file(filename);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string source = buffer.str();
            
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto stmts = parser.parse();
            
            interpret(stmts);
        } else {
             std::cerr << "Runtime Error: Could not find module '" << imp->moduleName << "'" << std::endl;
        }
        return;
    }
    
    if (auto dest = std::dynamic_pointer_cast<DestructureStmt>(stmt)) {
        Value init = evaluate(dest->initializer);
        if (init.isList && init.listVal.size() >= dest->names.size()) {
             for (size_t i = 0; i < dest->names.size(); i++) {
                 environment->define(dest->names[i], init.listVal[i]);
             }
        } else {
             std::cerr << "Runtime Error: Destructuring mismatch or not a list." << std::endl;
        }
    }
    else if (auto varDecl = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
        Value val = {"", 0, true};
        if (varDecl->initializer) val = evaluate(varDecl->initializer);
        environment->define(varDecl->name, val); // Define in current scope
    }
    else if (auto ret = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
         lastReturnValue = evaluate(ret->value);
         isReturning = true;
    }
    else if (auto funcDecl = std::dynamic_pointer_cast<FuncDeclStmt>(stmt)) {
        // Store as Value (Closure) in current scope
        // Capture CURRENT environment and Params
        environment->define(funcDecl->name, Value(funcDecl->body, environment, funcDecl->params));
    }
    else if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        executeBlock(block, std::make_shared<Environment>(environment));
    }
    else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        Value cond = evaluate(ifStmt->condition);
        bool isTrue = (cond.isInt && cond.intVal != 0) || (!cond.isInt && !cond.strVal.empty());
        
        if (isTrue) execute(ifStmt->thenBranch);
        else if (ifStmt->elseBranch) execute(ifStmt->elseBranch);
    }
    else if (auto switchStmt = std::dynamic_pointer_cast<SwitchStmt>(stmt)) {
        Value val = evaluate(switchStmt->condition);
        bool matchFound = false;
        
        // Helper to execute case body (which is Stmt, not Value)
        auto execStmt = [&](std::shared_ptr<Stmt> body) {
             if (auto block = std::dynamic_pointer_cast<BlockStmt>(body)) {
                 // For switch cases in same scope, we might just executeBlock? 
                 // Or create scope?
                 // Standard is: Switch shares scope or block scope per case?
                 // Let's create block scope.
                 executeBlock(block, std::make_shared<Environment>(environment));
             }
        };
        
        for (auto& cs : switchStmt->cases) {
            if (cs.value) { // Case
                Value caseVal = evaluate(cs.value);
                bool eq = false;
                if (val.isInt && caseVal.isInt) eq = (val.intVal == caseVal.intVal);
                else if (!val.isInt && !caseVal.isInt) eq = (val.strVal == caseVal.strVal);
                
                if (eq) {
                    execStmt(cs.body); 
                    matchFound = true;
                    break;
                }
            }
        }
        
        if (!matchFound) {
            for (auto& cs : switchStmt->cases) {
                if (!cs.value) { 
                     execStmt(cs.body);
                     break;
                }
            }
        }
    }
    else if (auto exprStmt = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
        evaluate(exprStmt->expr);
    }
}




void Interpreter::executeBlock(std::shared_ptr<BlockStmt> block, std::shared_ptr<Environment> env) {
    std::shared_ptr<Environment> previous = environment;
    environment = env;
    
    for (auto& s : block->statements) {
        execute(s);
        if (isReturning) break;
    }
    
    environment = previous;
}

Value Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    if (auto lit = std::dynamic_pointer_cast<LiteralExpr>(expr)) {
        if (lit->isString) return {lit->value, 0, false};
        return {"", std::stoi(lit->value), true};
    }
    if (auto var = std::dynamic_pointer_cast<VarExpr>(expr)) {
        return getVar(var->name);
    }
    if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
        std::vector<Value> args;
        for (auto& arg : call->args) args.push_back(evaluate(arg));
        
        if (natives.count(call->callee)) {
            return natives[call->callee](args);
        }
        
        // Closures
        Value v = getVar(call->callee);
        if (v.isClosure) {
            return callClosure(v, args);
        }
        
        return {"", 0, true}; // Void return (or undefined?)
    }
    if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        if (bin->op == "+=") {
            if (auto var = std::dynamic_pointer_cast<VarExpr>(bin->left)) {
                 Value r = evaluate(bin->right);
                 Value l = getVar(var->name); 
                 if (l.isInt && r.isInt) {
                     Value newVal("", l.intVal + r.intVal, true);
                     setVar(var->name, newVal);
                     return newVal;
                 }
            }
        }
        if (bin->op == "=") {
            if (auto var = std::dynamic_pointer_cast<VarExpr>(bin->left)) {
                 Value val = evaluate(bin->right);
                 setVar(var->name, val);
                 return val;
            }
        }
        
        Value l = evaluate(bin->left);
        Value r = evaluate(bin->right);
        
        if (bin->op == "==") {
             bool eq = (l.isInt == r.isInt) && (l.intVal == r.intVal) && (l.strVal == r.strVal);
             return {"", eq ? 1 : 0, true};
        }
        if (bin->op == "+") {
             if (l.isInt && r.isInt) return {"", l.intVal + r.intVal, true};
             return {l.toString() + r.toString(), 0, false};
        }
    }
    
    // Function Expression (Lambda)
    if (auto func = std::dynamic_pointer_cast<FunctionExpr>(expr)) {
        return Value(func->body, environment, func->params);
    }
    
    if (auto jsx = std::dynamic_pointer_cast<JsxExpr>(expr)) {
        // Component Expansion?
        // Check if tagName is a variable (function) in scope
        Value v = getVar(jsx->tagName);
        if (v.isClosure) {
             // User Component
             // Call it
             Value ret = callClosure(v, {}); // Use Value
             return ret;
        }
        
        std::string xml = "<" + jsx->tagName;
        for (auto const& [key, valExpr] : jsx->attributes) {
             Value attrVal = evaluate(valExpr);
             if (key.substr(0, 2) == "on" && attrVal.isClosure) {
                 std::string id = "cb_" + std::to_string((uintptr_t)attrVal.closureBody.get());
                 if (natives.count("bind_native_click")) {
                     natives["bind_native_click"]({Value(id, 0, false), attrVal});
                 }
                 xml += " " + key + "=\"" + id + "\"";
                 continue;
             }
             xml += " " + key + "=\"" + attrVal.toString() + "\"";
         }
        // ... children logic
        if (jsx->children.empty()) {
            xml += " />";
        } else {
            xml += ">";
            for(auto c : jsx->children) {
                 Value cv = evaluate(c);
                 xml += cv.toString();
            }
            xml += "</" + jsx->tagName + ">";
        }
        return {xml, 0, false};
    }
    
    return {"", 0, true};
}

// New method to replace callClosure logic properly
Value Interpreter::callClosure(Value closure, std::vector<Value> args) {
    if (!closure.isClosure || !closure.closureBody) return {"", 0, false};
    
    if (auto block = std::dynamic_pointer_cast<BlockStmt>(closure.closureBody)) {
        // Prepare Environment
        std::shared_ptr<Environment> prev = environment;
        // Use captured env as parent, create new scope
        if (closure.closureEnv) {
            environment = std::make_shared<Environment>(closure.closureEnv);
        } else {
            // Fallback (shouldn't happen if we capture correctly)
            environment = std::make_shared<Environment>(globals); 
        }
        
        // Bind Params
        for (size_t i = 0; i < closure.closureParams.size() && i < args.size(); i++) {
            environment->define(closure.closureParams[i], args[i]);
        }
        
        isReturning = false;
        executeBlock(block, environment); 
        
        Value ret = {"", 0, true};
        if (isReturning) ret = lastReturnValue;
        isReturning = false;
        
        environment = prev; // Restore
        return ret;
    }
    return {"", 0, true};
}

void Interpreter::executeClosure(Value closure, std::vector<Value> args) {
    if (!closure.isClosure || !closure.closureBody) return;
    
    if (auto block = std::dynamic_pointer_cast<BlockStmt>(closure.closureBody)) {
        std::shared_ptr<Environment> prev = environment;
        if (closure.closureEnv) {
            environment = std::make_shared<Environment>(closure.closureEnv);
        } else {
            environment = std::make_shared<Environment>(globals);
        }
        
        // Bind Params
        for (size_t i = 0; i < closure.closureParams.size() && i < args.size(); i++) {
            environment->define(closure.closureParams[i], args[i]);
        }
        
        executeBlock(block, environment);
        
        environment = prev;
    }
}

Value Interpreter::getGlobal(std::string name) {
    return globals->get(name);
}

void Interpreter::callFunction(std::string name) {
    // Deprecated
}

