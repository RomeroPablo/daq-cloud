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
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    sockaddr_in client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(CLIENT_PORT);
    if(bind(client_fd, (sockaddr*)&client_addr, sizeof(client_addr))){
        perror("client bind");
    }
    listen(client_fd, CLIENT_BACKLOG);
    std::cout << "[+] listening for clients on port " << CLIENT_PORT << "\n";

    // check for new connections, add them to a buffer
    while(true){
        int

    }
}


#define SOURCE_PORT 5600
void source_init(){

}

int main(void){

}

// fundamentally, what do we want?
// we want a separate thread that listens for, and adds clients to our buffer of clients
// once that is running, we should initialize our thread that sets up/looks for a source
// once the source is connected, we distribute our information to all of our connected clients
// if a client does not receive the message/disconencts, we remove it from our list
