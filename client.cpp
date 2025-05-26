// tcp_client.cpp
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <cstring>

int main() {
    const char* SERVER_IP = "3.129.249.33";
    const int PORT = 5000;

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
    buf_size = snprintf((char *)buffer, sizeof(buffer), "count == %i\n", count);
    sleep(3);
    send(sock, buffer, buf_size, 0);
    std::cout << "Message sent.\n";

    close(sock);
    return 0;
}
