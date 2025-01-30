#include "message.hpp"
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("filename.dat", std::ios::binary);
    
    if (!in) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    while (true) {
        my::message msg;
        in >> msg;

        if (in) {
            // Successfully read a message; print it
            std::cout << msg << "\n";
        }

        if (in.eof()) {
            break;  // Stop if EOF is reached *after* processing the last message
        }

        if (!in) {
            std::cerr << "Error reading message. Searching for next sync word...\n";
            in.clear();
            my::resynchronize(in);  // Attempt to find the next valid message
        }
    }

    return 0;
}
