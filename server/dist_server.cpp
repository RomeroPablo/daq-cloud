#include <asm-generic/socket.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <ostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <vector>

#define CLIENT_PORT 5700
#define CLIENT_BACKLOG 16
std::vector<int> clients;
std::mutex client_lock;
void client_addr(){
    // IPV4 & two way byte stream
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    sockaddr_in client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(CLIENT_PORT);
    if(bind(client_socket, (sockaddr*)&client_addr, sizeof(client_addr))){
        perror("client bind");
    }
    listen(client_socket, CLIENT_BACKLOG);
    std::cout << "[+] listening for clients on port " << CLIENT_PORT << "\n";

    // check for new connections, add them to a buffer
    int clients_size = 0;
    while(true){
        int client_fd = accept(client_socket, nullptr, nullptr);
        if (client_fd < 0){
            perror("client accept");
            continue;
        }
        {
            std::lock_guard lock(client_lock);
            clients.push_back(client_fd);
            clients_size = clients.size();
        } 
        std::cout << "[+] New client fd= " << client_fd << " (total: " << clients_size << ")\n";
    }
}


#define SOURCE_PORT 5600
void source_init(){
    int source_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt_val = 1;
    setsockopt(source_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    sockaddr_in source_addr;
    source_addr.sin_family = AF_INET;
    source_addr.sin_addr.s_addr = INADDR_ANY;
    source_addr.sin_port = htons(SOURCE_PORT);

    if(bind(source_socket, (sockaddr*)&source_addr, sizeof(source_addr)) < 0){
        perror("source bind");
    }
    listen(source_socket, 1);

    std::cout << "[+] Waiting for source on port " << SOURCE_PORT << "...\n";
    int source_fd = 0;
    while(source_fd <= 0) {
        sleep(1);
        source_fd = accept(source_socket, nullptr, nullptr);
    }
    std::cout << "[+] Source connected, fd= " << source_fd <<"\n";

}

int main(void){
    std::thread{client_addr}.detach();

    source_init();
}
