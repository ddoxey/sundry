#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>
#include <cstdint>

namespace my {

// Constants for message structure
constexpr size_t SYNC_WORD_SIZE = 4;       // Sync word (FA FA F0 F0)
constexpr size_t HEADER_SIZE = 11;         // Bytes before message size field
constexpr size_t SIZE_FIELD_SIZE = 2;      // Message size field (2 bytes)
constexpr size_t TOTAL_HEADER_SIZE = SYNC_WORD_SIZE + HEADER_SIZE + SIZE_FIELD_SIZE;  // 17 bytes

class message {
private:
    std::vector<uint8_t> raw_data;  // Stores the raw binary message
    uint16_t total_size = 0;        // Extracted total message size

public:
    message() = default;

    // Read binary data from an input stream
    friend std::istream& operator>>(std::istream& is, message& msg);

    // Print the message as C++ source code with a std::vector<uint8_t>
    friend std::ostream& operator<<(std::ostream& os, const message& msg);
};

// Function to resynchronize the input stream to the next sync word
void resynchronize(std::istream& in);

}  // namespace my

#endif  // MESSAGE_HPP
