/*
#include <uwebsockets/App.h>

int main() {
    uWS::App().ws<false>("/*", {
        .open = [](auto* ws) {
            std::cout << "Client connected" << std::endl;
        },
        .message = [](auto* ws, std::string_view msg, uWS::OpCode) {
            std::cout << "Received: " << msg << std::endl;
            ws->send("Echo: " + std::string(msg), uWS::OpCode::TEXT);
        },
        .close = [](auto* ws, int code, std::string_view msg) {
            std::cout << "Client disconnected" << std::endl;
        }
    }).listen(9001, [](auto* token) {
        if (token) std::cout << "Listening on port 9001" << std::endl;
    }).run();
}
*/
#include <asm-generic/socket.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

int main(){
    const int PORT = 5000;
    int server_fd, client_socket;
    sockaddr_in address{};
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //0.0.0.0
    address.sin_port = htons(PORT);

    if(bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }



}

/*
    listen(server_fd, 3);
    std::cout << "Server listening on port " << PORT << std::endl;

    client_socket = accept(server_fd, (sockaddr*)&address, &addrlen);
    if (client_socket < 0) {
        perror("accept failed");
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    while (true) {
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;
        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    close(client_socket);
    close(server_fd);
    return 0;
}
*/
