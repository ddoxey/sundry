/***
	Write a C++20 source file named `message.cpp` that implements a `my::message` class. The class reads structured binary messages from an input stream (`std::ifstream`) and provides a human-readable debugging output.

	### **Binary Message Format**
	1. The message contains a **sync word** (`FA FA F0 F0`) that marks the start.
	2. Any irrelevant bytes before the sync word should be ignored.
	3. After the sync word:
	- **Next 11 bytes** are a fixed header.
	- **Next 2 bytes** represent the total message size (big-endian).
	- **Remaining bytes** complete the message.

	### **Functionality**
	- **`operator>>` (istream)**: Reads a message, searching for the sync word and parsing the structure.
	- **`operator<<` (ostream)**: Outputs the message as **C++ source code**, using a `std::vector<uint8_t>` to initialize the byte sequence.
	- **Error Handling**: If a message is malformed, clear the error state and skip a byte to realign.

	### **Expected Output Format**
	If a message is successfully parsed, it should be printed like this:
	```cpp
	std::vector<uint8_t> buffer = {
		0xfa, 0xfa, 0xf0, 0xf0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 
		0x15, 0x16, 0x17, 0x18
	};
*/
#include "message.hpp"
#include <iomanip>
#include <sstream>

namespace my {

// Helper function to find the sync word (FA FA F0 F0) in the stream
bool find_sync(std::istream& is) {
    uint8_t buffer[4];
    
    // Read one byte at a time, shifting the buffer
    while (is.read(reinterpret_cast<char*>(&buffer[3]), 1)) {
        // Shift bytes left
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];

        // Check for sync word
        if (buffer[0] == 0xFA && buffer[1] == 0xFA && 
            buffer[2] == 0xF0 && buffer[3] == 0xF0) {
            return true;
        }
    }
    return false;  // Sync word not found
}

// Overload >> to read a message from an input stream, skipping irrelevant headers
std::istream& operator>>(std::istream& is, message& msg) {
    msg.raw_data.clear();

    // Find the first valid sync word
    if (!find_sync(is)) {
        is.setstate(std::ios::failbit);
        return is;
    }

    // Store the sync sequence
    msg.raw_data.insert(msg.raw_data.end(), {0xFA, 0xFA, 0xF0, 0xF0});

    // Read the next 11 bytes to complete the first section (total 15 bytes)
    uint8_t header[11];
    if (!is.read(reinterpret_cast<char*>(header), 11)) {
        return is;
    }
    msg.raw_data.insert(msg.raw_data.end(), header, header + 11);

    // Read the size field (2 bytes)
    uint8_t size_bytes[2];
    if (!is.read(reinterpret_cast<char*>(size_bytes), 2)) {
        return is;
    }
    msg.total_size = (size_bytes[0] << 8) | size_bytes[1];

    // Store size field
    msg.raw_data.insert(msg.raw_data.end(), size_bytes, size_bytes + 2);

    // Read remaining bytes based on total size
    size_t remaining = msg.total_size - 17;
    if (remaining > 0) {
        msg.raw_data.resize(msg.total_size);
        if (!is.read(reinterpret_cast<char*>(msg.raw_data.data() + 17), remaining)) {
            is.setstate(std::ios::failbit);
        }
    }

    return is;
}

// Overload << to output message as C++ vector source code
std::ostream& operator<<(std::ostream& os, const message& msg) {
    if (msg.raw_data.empty()) {
        return os << "std::vector<uint8_t> buffer = {}; // Empty message\n";
    }

    std::ostringstream formatted;
    formatted << "std::vector<uint8_t> buffer = {\n    ";

    for (size_t i = 0; i < msg.raw_data.size(); ++i) {
        formatted << "0x" 
                  << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(msg.raw_data[i]);

        if (i < msg.raw_data.size() - 1) {
            formatted << ", ";
        }

        // Wrap lines every 12 bytes for readability
        if ((i + 1) % 12 == 0) {
            formatted << "\n    ";
        }
    }

    formatted << "\n};\n";
    return os << formatted.str();
}

}  // namespace my
