#include <asm-generic/socket.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <ostream>
#include <unistd.h>
#include <cstring>
#include <thread>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <vector>

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

#define SOURCE_PORT 5600
#define BUFF_SIZE 1024
int main(){
    // initialize client acceptance thread
    std::thread(client_acceptor).detach();

    // configure src/generator port
    int src_listen = socket(AF_INET, SOCK_STREAM, 0);
    if(src_listen < 0) { perror("source socket"); return 1; }
    sockaddr_in src_addr{};
    src_addr.sin_family         = AF_INET;
    src_addr.sin_addr.s_addr    = INADDR_ANY;
    src_addr.sin_port           = htons(SOURCE_PORT);
    int opt = 1;
    setsockopt(src_listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(bind(src_listen, (sockaddr*)&src_addr, sizeof(src_addr)) < 0){
        perror("source bind"); return 1;
    }
    int src_fd = accept(src_listen, nullptr, nullptr);
    if(src_fd < 0) { perror("source accept"); return 1; }
    std::cout << "[+] Source connected, fd=" << src_fd << "\n";

    // read and broadcast to clients
    char buffer[BUFF_SIZE];
    while(true){
        ssize_t n = read(src_fd, buffer, sizeof(buffer));
        if (n <= 0){
            std::cout << "[!] Source disconnected or error\n";
            break;
        }

        // broadcast to all clients, remove if it fails
        std::vector<int> to_remove;
        {
            std::lock_guard<std::mutex> lock(clients_mtx);
            for(int cfd : clients){
                if(send(cfd, buffer, n, 0) <= 0){
                    to_remove.push_back(cfd);
                }
            }

            for(int bad : to_remove){
                close(bad);
                clients.erase(std::remove(clients.begin(), clients.end(), bad), clients.end());
                std::cout << "[!] Removed client fd=" << bad << " (now " << clients.size() << " left\n";
            }
        }

    }

    close(src_listen);
    close(src_fd);
    return 0;
}
