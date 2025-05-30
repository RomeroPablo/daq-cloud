#include <asm-generic/socket.h>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <ostream>
#include <unistd.h>
#include <cstring>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <vector>

int main(){
    const int PORT = 5600;
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

    listen(server_fd, 3);
    std::cout << "Server listening on port " << PORT << std::endl;

    client_socket = accept(server_fd, (sockaddr*)&address, &addrlen);
    if(client_socket < 0){
        perror("accept failed");
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    while(true) {
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;
        buffer[bytes_read] = '\0';
        std::cout << buffer << std::endl;
    }

    close(client_socket);
    close(server_fd);
    return 0;
}

#define CLIENT_PORT 5700
#define BACKLOG 16
std::mutex clients_mtx;
std::vector<int> clients;

void client_acceptor() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("client socket"); return; }
    
    sockaddr_in addr{};
    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = INADDR_ANY;
    addr.sin_port           = htons(CLIENT_PORT);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(listen_fd, (sockaddr*)&addr, sizeof(addr))){
        perror("client bind"); return;
    }
    listen(listen_fd, BACKLOG);
    std::cout << "[+] Listening for clients on port " << CLIENT_PORT << "\n";

    while(true){
        int client_fd = accept(listen_fd, nullptr, nullptr); // can extract addr here
        if(client_fd < 0){
            perror("client accept");
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(clients_mtx);
            clients.push_back(client_fd);
        }
        std::cout << "[+] New client: fd=" << client_fd << " (total: " << clients.size() << ")\n";
    }


}
