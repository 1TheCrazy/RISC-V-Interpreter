#include <cstdint>
#include <objects/ELF.h>
#include <objects/Instructions.h>

class Program{
    public:
        // Fixed size array for now
        uint8_t RAM[200000];
        uint64_t REG[33];
        uint64_t program_counter;

        // Step by one simulated "clock cycle"
        void step();
        // Setup: load the program into RAM, ...
        Program(const ELF& elf, const std::vector<uint8_t>& bytes);

    private:
        Instrcution get_instruction_from_32(const uint32_t& instruction);
        void execute_instruction(Instrcution instrcution, const uint32_t& data);
};