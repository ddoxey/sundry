/**
    Write a C++20 source file named `main.cpp` that reads binary messages from a file using the `my::message` class.

    ### **Functionality**
    1. Open a binary file named `"filename.dat"` using `std::ifstream` in **binary mode**.
    2. Continuously read `my::message` objects from the file **until EOF**.
    3. Each successfully read message should be printed using `operator<<`, which formats it as **C++ source code** with `std::vector<uint8_t>`.
    4. If an error occurs while reading a message, print `"Error reading message. Skipping..."`, **clear the stream state**, and skip 1 byte (`in.ignore(1)`) to attempt resynchronization.
    5. Ensure that if the last message reaches EOF, it **still gets printed** before exiting.

    ### **Expected Behavior**
    - **Valid messages** will be printed as C++ `std::vector<uint8_t>` definitions.
    - **Invalid messages** will cause an error message and trigger a resynchronization attempt.
    - **The program gracefully exits** when EOF is reached.

    ### **Implementation**
    Write the full `main.cpp` file to accomplish this.
 */
#include "message.hpp"
#include <fstream>

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
            std::cerr << "Error reading message. Skipping...\n";
            in.clear();  // Clear error flags
            in.ignore(1);  // Move past the problematic byte and continue
        }
    }

    return 0;
}
