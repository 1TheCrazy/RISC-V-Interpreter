#include <iostream>
using namespace std;

#include <cstdint>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>

// Util for reading a file in bytes
std::vector<std::uint8_t> read_file_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Failed to open file: " + path);

    f.seekg(0, std::ios::end);
    const std::streamsize size = f.tellg();
    if (size < 0) throw std::runtime_error("tellg failed");

    std::vector<std::uint8_t> data(static_cast<size_t>(size));

    f.seekg(0, std::ios::beg);
    if (!f.read(reinterpret_cast<char*>(data.data()), size))
        throw std::runtime_error("Failed to read file: " + path);

    return data;
}

int main(){
    try {
        auto bytes = read_file_bytes("./doom-riscv.elf");

        // print bytes (hex)
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            std::cout << std::hex
                      << std::setw(2)
                      << std::setfill('0')
                      << static_cast<int>(bytes[i])
                      << ' ';
        }

        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}