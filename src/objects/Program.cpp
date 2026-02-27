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
    auto opcode = op_code_of(inst);

    InstructionGroup instr_group = static_cast<InstructionGroup>(opcode);
    InstructionType instr_type = instruction_group_to_type(instr_group);
    Instruction instruction = get_instruction(instr_type, inst);

    return instruction;
}

void Program::execute_instruction(Instruction instruction, const uint32_t& data){
    switch(instruction){
        case Instruction::ADD:
            REG[rd_of(data)] = REG[rs1_of(data)] + REG[rs2_of(data)];
            break;
        case Instruction::ADDI:
            REG[rd_of(data)] = REG[rs1_of(data)] + imm_11_0_of(data);
            break;
        case Instruction::SUB:
            REG[rd_of(data)] = REG[rs1_of(data)] - REG[rs2_of(data)];
        case Instruction::AND:
            REG[rd_of(data)] = REG[rs1_of(data)] & REG[rs2_of(data)];
        case Instruction::ANDI:
            REG[rd_of(data)] = REG[rs1_of(data)] & imm_11_0_of(data);
        case Instruction::OR:
            REG[rd_of(data)] = REG[rs1_of(data)] | REG[rs2_of(data)];
        case Instruction::ORI:
            REG[rd_of(data)] = REG[rs1_of(data)] | imm_11_0_of(data);
        case Instruction::XOR:
            REG[rd_of(data)] = REG[rs1_of(data)] ^ REG[rs2_of(data)];
        case Instruction::XORI:
            REG[rd_of(data)] = REG[rs1_of(data)] ^ imm_11_0_of(data);
        case Instruction::SLL:
            REG[rd_of(data)] = REG[rs1_of(data)] << REG[rs2_of(data)];
        case Instruction::SLLI:
            REG[rd_of(data)] = REG[rs1_of(data)]  << shamt_of(data); // vielleicht falsch bis komisch??
        case Instruction::SRL:
            REG[rd_of(data)] = REG[rs1_of(data)] >> REG[rs2_of(data)];
        case Instruction::SRLI:
            REG[rd_of(data)] = REG[rs1_of(data)]  >> shamt_of(data); // vielleicht falsch bis komisch??
        
        
        
        
        

    }
}
