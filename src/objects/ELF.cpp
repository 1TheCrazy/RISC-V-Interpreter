#include <vector>
#include "objects/ELF.h"
#include "cstdint"

ELF::ELF(const std::vector<uint8_t>& bytes)
{
    elf_header.fill(bytes);
    auto program_start = set_program_headers(bytes);
    elf_header.programm_start = program_start; 
}

uint64_t ELF::set_program_headers(const std::vector<uint8_t>& bytes){
    uint64_t programm_start = 0xFFFFFFFFFFFFFFFF;

    for(int i = 0; i < elf_header.e_phenum; i++){
        ProgramHeader p;
        p.fill(bytes,elf_header.e_phoff+ i * elf_header.e_phentsize);

        if(p.p_vaddr < programm_start)
            programm_start = p.p_vaddr;

        program_headers.push_back(p);
    }

    return programm_start;
}      
