#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int PORT = 5556;

// Function to get current date and time as a string
std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_struct = *std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr{}, client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    std::vector<char> spinner = {'-', '\\', '|', '/'};
    int spin_index = 0;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        return EXIT_FAILURE;
    }

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return EXIT_FAILURE;
    }

    // Listen for connections
    if (listen(server_fd, 1) < 0) {
        return EXIT_FAILURE;
    }

    while (true) {
        // Accept client connection
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            continue;
        }

        // Maintain connection, sending timestamps periodically
        while (true) {
            std::string datetime = get_current_time() + "\n";
            ssize_t sent_bytes = send(client_fd, datetime.c_str(), datetime.size(), 0);
            if (sent_bytes < 0) {
                break; // Client likely disconnected
            }

            // Display spinner animation
            std::cout << spinner[spin_index] << "\b" << std::flush;
            spin_index = (spin_index + 1) % spinner.size();

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        close(client_fd); // Close client connection before accepting a new one
    }

    close(server_fd);
    return 0;
}
