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

uint64_t get_e_entry(vector<uint8_t> bytes){
    // Skip to e_entry as per spec
    int pointer = 24;
    uint64_t e_entry = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 8; i++){
        e_entry |= bytes[pointer] << i * 8;
        pointer++;    
    }

    return e_entry;
}

int main(){

    auto bytes = read_file_bytes("./doom-riscv.elf");
    int64_t e_entry = get_e_entry(bytes);
    std::cout << e_entry;

    return 0;
}