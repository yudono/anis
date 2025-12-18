#ifndef SUNDA_TCP_SERVER_H
#define SUNDA_TCP_SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace WebServer {

class TCPServer {
private:
    int server_fd = -1;
    int port = 3000;
    bool running = false;

public:
    TCPServer() {}
    ~TCPServer() { stop(); }

    bool start(int p) {
        port = p;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) return false;

        // Allow address reuse
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            close(server_fd);
            return false;
        }

        if (listen(server_fd, 10) < 0) {
            close(server_fd);
            return false;
        }

        running = true;
        return true;
    }

    void stop() {
        if (server_fd >= 0) {
            close(server_fd);
            server_fd = -1;
        }
        running = false;
    }

    int accept_client() {
        if (!running) return -1;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        return accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    }

    std::string read_request(int client_fd) {
        char buffer[4096] = {0};
        int valread = read(client_fd, buffer, sizeof(buffer));
        if (valread <= 0) return "";
        return std::string(buffer, valread);
    }

    void send_response(int client_fd, const std::string& response) {
        write(client_fd, response.c_str(), response.length());
    }

    void close_client(int client_fd) {
        close(client_fd);
    }
};

} // namespace WebServer

#endif
