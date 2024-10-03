#include "file_utils.h"
#include <fstream>
#include <stdexcept>

namespace file_utils {
    std::vector<char> readFile(const std::string& filepath) {
        // Open the file as a binary stream
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        // Get the file size and allocate the buffer
        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        // Move the pointer back to the beginning of the file and read the file into the buffer
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }
}
