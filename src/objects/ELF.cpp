#include <vector>
#include "objects/ELF.h"
#include "cstdint"

ELF::ELF(const std::vector<uint8_t>& bytes)
{
    elf_header.fill(bytes);
    set_program_headers(bytes);
}

void ELF::set_program_headers(const std::vector<uint8_t>& bytes){
    for(int i = 0; i < elf_header.e_phenum; i++){
        ProgramHeader p;
        p.fill(bytes, elf_header.e_phoff + (i + 1 ) * elf_header.e_phentsize);
        program_headers.push_back(p);
    }
}      
