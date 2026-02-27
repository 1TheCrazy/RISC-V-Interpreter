#include <objects/Program.h>
#include <objects/Instructions.h>
#include <objects/InstructionType.h>
#include <vector>
#include <iostream>
#include <util/InstructionUtil.h>

Program::Program(const ELF& elf, const std::vector<uint8_t>& bytes){
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
    uint32_t instruction = 0;
    
    for(int i = 0; i < 4; i++){
        instruction |= uint32_t(RAM[program_counter]) << i * 8;
        program_counter++;    
    }
    
    Instruction instr = get_instruction_from_32(instruction);
    execute_instruction(instr, instruction); 
}

Instruction Program::get_instruction_from_32(const uint32_t& inst){
    auto opcode = inst & 0b1111111;
    auto func_3 = (inst >> 11) & 0b111;
    auto func7 = inst >> 24;

    InstructionGroup instr_group = static_cast<InstructionGroup>(opcode);
    InstructionType instr_type = instruction_group_to_type(instr_group);
    Instruction instruction = get_instruction(instr_type, inst);

    // Process that instruction here
}

void Program::execute_instruction(Instruction instruction, const uint32_t& data){
    switch(instruction){
        // Add cases here
    }
}