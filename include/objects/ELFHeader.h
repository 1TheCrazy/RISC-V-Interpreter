#include <cstdint>

#pragma once

struct ELFHeader{
    bool is_64;
    bool is_little_endian;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint16_t e_phentsize;
    uint16_t e_phenum;
    uint64_t programm_start;

    void fill(const std::vector<uint8_t>& bytes);
};