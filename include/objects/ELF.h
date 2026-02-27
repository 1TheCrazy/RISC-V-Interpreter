#include <vector>
#include "objects/ProgramHeader.h"
#include "objects/ELFHeader.h"

using namespace std;

#pragma once

class ELF{
    public:
        vector<ProgramHeader> program_headers;
        ELFHeader elf_header;
        ELF(const std::vector<uint8_t>& bytes);

    private:
        void set_program_headers(const std::vector<uint8_t>& bytes);
};