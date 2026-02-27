#include <objects/Program.h>
#include <vector>

void Program::init(const ELF& elf, const std::vector<uint8_t>& bytes){
    for(int i = 0; i < elf.program_headers.size(); i++){
        auto header = elf.program_headers[i];
        
        if(header.p_memsz == 0) continue;

        for(int i = 0; i < header.p_filesz; i++){
            RAM[header.p_vaddr + i] = bytes[header.p_offset + i];
        }
    }

    program_counter = elf.elf_header.e_entry;
}

void Program::step(){
    
}