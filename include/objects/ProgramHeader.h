#include <cstdint>

struct ProgramHeader{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;

    void fill(const std::vector<uint8_t>& bytes, uint64_t offset);
};