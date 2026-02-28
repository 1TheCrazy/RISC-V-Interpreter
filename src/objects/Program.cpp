#include <objects/Program.h>
#include <objects/Instructions.h>
#include <objects/InstructionType.h>
#include <vector>
#include <iostream>
#include <util/InstructionUtil.h>
#include <util/SignUtil.h>

Program::Program(const ELF& elf, const std::vector<uint8_t>& bytes){
    for(int i = 0; i < elf.program_headers.size(); i++){
        auto header = elf.program_headers[i];
        
        if(header.p_memsz == 0) continue;

        for(int i = 0; i < header.p_filesz; i++){
            RAM[header.p_vaddr + i] = bytes[header.p_offset + i];
        }
    }

    program_counter = elf.elf_header.e_entry;

    // Set Stack Pointer
    REG[2] = elf.elf_header.programm_start;
    
    // Run the program on start
    running = true;
}

void Program::step(){
    // Follow conventions
    REG[0] = 0x0;

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
            REG[rd_of(data)] = REG[rs1_of(data)] + sign_extend<int64_t>(imm_11_0_of(data), 12);
            break;

        case Instruction::SUB:
            REG[rd_of(data)] = REG[rs1_of(data)] - REG[rs2_of(data)];
            break;

        case Instruction::AND:
            REG[rd_of(data)] = REG[rs1_of(data)] & REG[rs2_of(data)];
            break;

        case Instruction::ANDI:
            REG[rd_of(data)] = REG[rs1_of(data)] & static_cast<uint64_t>(sign_extend<int64_t>(imm_11_0_of(data), 12));
            break;

        case Instruction::OR:
            REG[rd_of(data)] = REG[rs1_of(data)] | REG[rs2_of(data)];
            break;

        case Instruction::ORI:
            REG[rd_of(data)] = REG[rs1_of(data)] | static_cast<uint64_t>(sign_extend<int64_t>(imm_11_0_of(data), 12));
            break;

        case Instruction::XOR:
            REG[rd_of(data)] = REG[rs1_of(data)] ^ REG[rs2_of(data)];
            break;

        case Instruction::XORI:
            REG[rd_of(data)] = REG[rs1_of(data)] ^ static_cast<uint64_t>(sign_extend<int64_t>(imm_11_0_of(data), 12));
            break;

        case Instruction::SLL: {
            uint64_t shamt = REG[rs2_of(data)] & 0x3F;      // RV64 mask
            REG[rd_of(data)] = REG[rs1_of(data)] << shamt;
            break;
        }

        case Instruction::SLLI: {
            uint64_t shamt = shamt_of(data) & 0x3F;         // RV64 mask
            REG[rd_of(data)] = REG[rs1_of(data)] << shamt;
            break;
        }

        case Instruction::SRL: {
            uint64_t shamt = REG[rs2_of(data)] & 0x3F;      // RV64 mask
            REG[rd_of(data)] = REG[rs1_of(data)] >> shamt;
            break;
        }

        case Instruction::SRLI: {
            uint64_t shamt = shamt_of(data) & 0x3F;         // RV64 mask
            REG[rd_of(data)] = REG[rs1_of(data)] >> shamt;
            break;
        }
        case Instruction::LB: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr  = REG[rs1_of(data)] + offset;
            int8_t b = static_cast<int8_t>(RAM[addr]);
            REG[rd_of(data)] = static_cast<int64_t>(b);
            break;
        }
        case Instruction::LH: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr  = REG[rs1_of(data)] + offset;
            uint16_t value = RAM[addr] |          // Low bit
                            (RAM[addr + 1] << 8); // High Bit
            REG[rd_of(data)] = sign_extend<int64_t>(value, 16);
            break;
        }
        case Instruction::LW: { 
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr  = REG[rs1_of(data)] + offset;
            uint16_t value = RAM[addr] |
                            (RAM[addr + 1] << 8) |
                            (RAM[addr + 1] << 16) |
                            (RAM[addr + 1] << 24);

            REG[rd_of(data)] = sign_extend<int64_t>(value, 32);
            break;
        }
        case Instruction::LHU: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr  = REG[rs1_of(data)] + offset;
            uint16_t value = static_cast<uint16_t>(static_cast<uint8_t>(RAM[addr])) |
                             (static_cast<uint16_t>(static_cast<uint8_t>(RAM[addr + 1])) << 8);
            REG[rd_of(data)] = static_cast<uint64_t>(value);
            break;
        }
        case Instruction::LBU: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr  = REG[rs1_of(data)] + offset;
            uint8_t value = RAM[addr];
            REG[rd_of(data)] = static_cast<uint64_t>(value);
            break;
        }
        case Instruction::SW: {
            int64_t  offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr   = REG[rs1_of(data)] + offset;

            uint32_t value = static_cast<uint32_t>(REG[rs2_of(data)]); // store low 32 bits

            RAM[addr + 0] = static_cast<uint8_t>( value        & 0xFF);
            RAM[addr + 1] = static_cast<uint8_t>((value >>  8) & 0xFF);
            RAM[addr + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
            RAM[addr + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);

            break;
        }
        case Instruction::SH: {
            int64_t  offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr   = REG[rs1_of(data)] + offset;

            uint16_t value = static_cast<uint16_t>(REG[rs2_of(data)]);

            RAM[addr + 0] = static_cast<uint8_t>( value        & 0xFF);
            RAM[addr + 1] = static_cast<uint8_t>((value >>  8) & 0xFF);

            break;
        }
        case Instruction::SB: {
            int64_t  offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr   = REG[rs1_of(data)] + offset;

            uint8_t value = static_cast<uint8_t>(REG[rs2_of(data)]);

            RAM[addr] = static_cast<uint8_t>(value);            
            break;
        }
        case Instruction::SRA:
            REG[rd_of(data)] = REG[rs1_of(data)] >> REG[rs2_of(data)];
            break;
        case Instruction::SRAI:   
            REG[rd_of(data)] = REG[rs1_of(data)]  >> shamt_of(data); // vielleicht falsch bis komisch??
            break;
        case Instruction::SLT:
            REG[rd_of(data)] = int32_t(REG[rs1_of(data)] < REG[rs2_of(data)]);
            break;
        case Instruction::SLTI:
            REG[rd_of(data)] = int32_t(REG[rs1_of(data)] < imm_11_0_of(data));
            break;
        case Instruction::SLTU: {
            uint32_t rs1 = rs1_of(data);
            uint32_t rs2 = rs2_of(data);
            REG[rd_of(data)] = int32_t(REG[rs1] < REG[rs2]);
            break;
        }
        case Instruction::SLTIU: {
            uint32_t rs1 = rs1_of(data);
            uint32_t imm = imm_11_0_of(data);
            REG[rd_of(data)] = int32_t(REG[rs1] < imm);
            break;
        }
        case Instruction::LUI:
            REG[rd_of(data)] = int32_t(imm_31_12_of(data) << 12);
            break;
        case Instruction::AUIPC:
            REG[rd_of(data)] = program_counter+(imm_31_12_of(data) << 12); //wen was komisch, hier prüfen
            break;
        case Instruction::JAL:{
            REG[rd_of(data)] = program_counter+4;
            program_counter = program_counter + imm_20_19_12_11_10_1_of(data);
            break;
        }
        case Instruction::JALR:
            REG[rd_of(data)] = program_counter+4;
            program_counter = rs1_of(data)+imm_11_0_of(data);
            break;
        case Instruction::BEQ:
            if(rs1_of(data) == rs2_of(data)){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        case Instruction::BNE:
            if(rs1_of(data) != rs2_of(data)){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        case Instruction::BLT:
            if(rs1_of(data) < rs2_of(data)){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        case Instruction::BGE:
            if(rs1_of(data) >= rs2_of(data)){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        case Instruction::BLTU:
            if(uint32_t(rs1_of(data)) < uint32_t(rs2_of(data))){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        case Instruction::BGEU:
            if(uint32_t(rs1_of(data)) >= uint32_t(rs2_of(data))){
                program_counter += imm_12_11_10_5_4_1_of(data);
            }
            break;
        


        
        case Instruction::ECALL:
            process_syscall();
            break;
    }
}

void Program::process_syscall(){
    // Get Syscall number
    uint64_t syscall = REG[17];

    uint64_t arg1 = REG[10];
    uint64_t arg2 = REG[11];
    uint64_t arg3 = REG[12];
    

    if(syscall == 66){
        auto place = arg2;
        int out_len = 0;

        for(int i=0; i <arg3; i++){
            uint64_t string_pointer = 0;
            uint64_t string_len = 0;

            for(int j=0;j<8; j++){
                string_pointer |= RAM[place + i*8*2 + j] << j *8;
            }

            for(int j=0;j<8; j++){
                string_len |= RAM[place + 8 + 8*i*2 + j] << j *8;
            }

            out_len += string_len;
            
            for(int k = 0; k< string_len; k++){
                char c = RAM[string_pointer + k];
                cout << c;
            }
            
        }

        // Save out 
        REG[10] = out_len;
    }
    else if(syscall == 93 || syscall == 94){
        running = false;
    }
}

void Program::start(){
    while(running){
        step();
    }
}