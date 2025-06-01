// source.cpp
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <cstring>
#include "ip_key.h"

int main() {
    const int PORT = 5600;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return 1;
    }

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Connected to server.\n";

    int count = 0;
    int buf_size = 0;
    uint8_t buffer[64];
    while(1){
        buf_size = snprintf((char *)buffer, sizeof(buffer), "count == %d", count);
        ssize_t amt_sent = send(sock, buffer, buf_size, 0);
        if (amt_sent < 0){
            std::perror("send");
        }
        std::cout << "Message sent: " << buffer << std::endl;
        count++;
        sleep(1);
    }
    close(sock);
    return 0;
}
