#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int PORT = 5557; // +1 where the proxy is listening
constexpr int BUFFER_SIZE = 1024;

int main() {
    int sock_fd;
    struct sockaddr_in server_addr{};
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return EXIT_FAILURE;
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return EXIT_FAILURE;
    }

    // Connect to the server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "Connected to server.\n";

    // Keep reading timestamps until the connection is closed
    while (true) {
        ssize_t bytes_received = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received: " << buffer;
        } else {
            std::cerr << "Server disconnected.\n";
            break;
        }
    }

    close(sock_fd);
    return 0;
}
