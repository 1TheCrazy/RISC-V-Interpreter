#include <cstdint>
#include <vector>

uint64_t get_64_by_offset(int pointer, const std::vector<uint8_t>& bytes);
uint32_t get_32_by_offset(int pointer, const std::vector<uint8_t>& bytes);
uint16_t get_16_by_offset(int pointer, const std::vector<uint8_t>& bytes);
bool is_elf_little_endian(const std::vector<uint8_t>& bytes);