#include <cstdint>
#include <objects/ELF.h>

class Program{
    // Fixed size array for now
    uint8_t RAM[200000];
    uint64_t REG[33];
    uint64_t program_counter;

    // Step by one simulated "clock cycle"
    void step();
    // Setup: load the program into RAM, ...
    void init(const ELF& elf, const std::vector<uint8_t>& bytes);
};