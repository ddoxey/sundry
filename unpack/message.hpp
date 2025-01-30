#include "message.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

namespace my {

// Helper function to find the sync word (FA FA F0 F0) in the stream
void resynchronize(std::istream& in) {
    uint8_t buffer[4];

    // Read the first 4 bytes upfront
    if (!in.read(reinterpret_cast<char*>(buffer), 4)) {
        return;  // Unable to read 4 bytes, probably EOF
    }

    size_t skipped = 0;

    // Scan for the sync word
    while (in.read(reinterpret_cast<char*>(&buffer[3]), 1)) {  // Read one new byte into buffer[3]
        skipped++;
        if (buffer[0] == 0xFA && buffer[1] == 0xFA &&
            buffer[2] == 0xF0 && buffer[3] == 0xF0) {
            // Found sync word, rewind 4 bytes to keep it aligned
            in.seekg(-4, std::ios::cur);
            std::cerr << "Resynchronized after skipping " << skipped << " bytes.\n";
            return;
        }

        // Slide window left correctly
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];  // Only update the first three positions
    }
}

// Overload >> to read a message from an input stream, skipping irrelevant headers
std::istream& operator>>(std::istream& is, message& msg) {
    msg.raw_data.clear();

    // Find the first valid sync word
    resynchronize(is);
    if (is.eof()) {
        return is;  // Stop reading if EOF is reached
    }

    // Store the sync sequence
    msg.raw_data.insert(msg.raw_data.end(), {0xFA, 0xFA, 0xF0, 0xF0});

    // Read the next TOTAL_HEADER_SIZE bytes before the message size field
    uint8_t header[my::HEADER_SIZE];
    if (!is.read(reinterpret_cast<char*>(header), my::HEADER_SIZE)) {
        return is;
    }
    msg.raw_data.insert(msg.raw_data.end(), header, header + my::HEADER_SIZE);

    // Read the size field (2 bytes)
    uint8_t size_bytes[my::SIZE_FIELD_SIZE];
    if (!is.read(reinterpret_cast<char*>(size_bytes), my::SIZE_FIELD_SIZE)) {
        return is;
    }

    // Compute total size
    msg.total_size = (size_bytes[0] << 8) | size_bytes[1];

    // Store size field
    msg.raw_data.insert(msg.raw_data.end(), size_bytes, size_bytes + my::SIZE_FIELD_SIZE);

    // Read remaining bytes based on total size
    size_t remaining = msg.total_size - my::TOTAL_HEADER_SIZE;
    if (remaining > 0) {
        msg.raw_data.resize(msg.total_size);
        if (!is.read(reinterpret_cast<char*>(msg.raw_data.data() + my::TOTAL_HEADER_SIZE), remaining)) {
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
