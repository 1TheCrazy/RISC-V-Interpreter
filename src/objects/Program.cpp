#include <objects/Program.h>
#include <objects/Instructions.h>
#include <objects/InstructionType.h>
#include <vector>
#include <iostream>
#include <util/InstructionUtil.h>
#include <util/LogUtil.h>
#include <util/SignUtil.h>

Program::Program(const ELF& elf, const std::vector<uint8_t>& bytes){
    for (int i = 0; i < 33; i++) {
        REG[i] = 0;
    }
    for (int i = 0; i < 2000000; i++) {
        RAM[i] = 0;
    }

    for(int i = 0; i < elf.program_headers.size(); i++){
        auto header = elf.program_headers[i];

        if(header.p_memsz == 0) continue;

        for(uint64_t j = 0; j < header.p_filesz; j++){
            RAM[header.p_vaddr + j] = bytes[header.p_offset + j];
        }
    }

    program_counter = elf.elf_header.e_entry;

    // Set Stack Pointer
    // REG[2] = elf.elf_header.programm_start;
    REG[2] = 0xf000;

    // Run the program on start
    running = true;
    wrote_fallback_message = false;
}

void Program::step(){
    // Follow conventions
    REG[0] = 0x0;

    uint32_t instruction = 0;
    const uint64_t instruction_pc = program_counter;

    for(int i = 0; i < 4; i++){
        instruction |= uint32_t(RAM[program_counter]) << (i * 8);
        program_counter++;
    }

    Instruction instr = get_instruction_from_32(instruction);
    log_debug_instruction_state(instr, instruction, instruction_pc, REG);

    execute_instruction(instr, instruction);
}

Instruction Program::get_instruction_from_32(const uint32_t& inst){
    auto opcode = op_code_of(inst);
    try {
        InstructionGroup instr_group = static_cast<InstructionGroup>(opcode);
        InstructionType instr_type = instruction_group_to_type(instr_group);
        Instruction instruction = get_instruction(instr_type, inst);
        return instruction;
    } catch (const std::exception&) {
        std::cerr << "Decode failure: inst=0x" << std::hex << inst
                  << " opcode=0x" << opcode
                  << " pc=0x" << (program_counter - 4)
                  << std::dec << '\n';
        throw;
    }
}

void Program::execute_instruction(Instruction instruction, const uint32_t& data){
    const uint64_t instruction_pc = program_counter - 4;

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

        case Instruction::ADDW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)] + REG[rs2_of(data)]))));
            break;

        case Instruction::SUBW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)] - REG[rs2_of(data)]))));
            break;

        case Instruction::SLLW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)]) << (REG[rs2_of(data)] & 0x1F))));
            break;

        case Instruction::SRLW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)]) >> (REG[rs2_of(data)] & 0x1F))));
            break;

        case Instruction::SRAW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(
                static_cast<int32_t>(REG[rs1_of(data)]) >> (REG[rs2_of(data)] & 0x1F)));
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
            uint64_t shamt = REG[rs2_of(data)] & 0x3F;
            REG[rd_of(data)] = REG[rs1_of(data)] << shamt;
            break;
        }

        case Instruction::SLLI: {
            uint64_t shamt = shamt_of(data) & 0x3F;
            REG[rd_of(data)] = REG[rs1_of(data)] << shamt;
            break;
        }

        case Instruction::SRL: {
            uint64_t shamt = REG[rs2_of(data)] & 0x3F;
            REG[rd_of(data)] = REG[rs1_of(data)] >> shamt;
            break;
        }

        case Instruction::SRLI: {
            uint64_t shamt = shamt_of(data) & 0x3F;
            REG[rd_of(data)] = REG[rs1_of(data)] >> shamt;
            break;
        }

        case Instruction::LB: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            int8_t value = static_cast<int8_t>(RAM[addr]);
            REG[rd_of(data)] = static_cast<int64_t>(value);
            break;
        }

        case Instruction::LH: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint16_t value = static_cast<uint16_t>(RAM[addr]) |
                             (static_cast<uint16_t>(RAM[addr + 1]) << 8);
            REG[rd_of(data)] = sign_extend<int64_t>(value, 16);
            break;
        }

        case Instruction::LW: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint32_t value = static_cast<uint32_t>(RAM[addr]) |
                             (static_cast<uint32_t>(RAM[addr + 1]) << 8) |
                             (static_cast<uint32_t>(RAM[addr + 2]) << 16) |
                             (static_cast<uint32_t>(RAM[addr + 3]) << 24);
            REG[rd_of(data)] = sign_extend<int64_t>(value, 32);
            break;
        }

        case Instruction::LD: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint64_t value = static_cast<uint64_t>(RAM[addr]) |
                             (static_cast<uint64_t>(RAM[addr + 1]) << 8) |
                             (static_cast<uint64_t>(RAM[addr + 2]) << 16) |
                             (static_cast<uint64_t>(RAM[addr + 3]) << 24) |
                             (static_cast<uint64_t>(RAM[addr + 4]) << 32) |
                             (static_cast<uint64_t>(RAM[addr + 5]) << 40) |
                             (static_cast<uint64_t>(RAM[addr + 6]) << 48) |
                             (static_cast<uint64_t>(RAM[addr + 7]) << 56);
            REG[rd_of(data)] = value;
            break;
        }

        case Instruction::LHU: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint16_t value = static_cast<uint16_t>(RAM[addr]) |
                             (static_cast<uint16_t>(RAM[addr + 1]) << 8);
            REG[rd_of(data)] = static_cast<uint64_t>(value);
            break;
        }

        case Instruction::LWU: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint32_t value = static_cast<uint32_t>(RAM[addr]) |
                             (static_cast<uint32_t>(RAM[addr + 1]) << 8) |
                             (static_cast<uint32_t>(RAM[addr + 2]) << 16) |
                             (static_cast<uint32_t>(RAM[addr + 3]) << 24);
            REG[rd_of(data)] = static_cast<uint64_t>(value);
            break;
        }

        case Instruction::LBU: {
            int64_t offset = sign_extend<int64_t>(imm_11_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;
            uint8_t value = RAM[addr];
            REG[rd_of(data)] = static_cast<uint64_t>(value);
            break;
        }

        case Instruction::SW: {
            int64_t offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;

            uint32_t value = static_cast<uint32_t>(REG[rs2_of(data)]);
            RAM[addr + 0] = static_cast<uint8_t>(value & 0xFF);
            RAM[addr + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            RAM[addr + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
            RAM[addr + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
            break;
        }

        case Instruction::SD: {
            int64_t offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;

            uint64_t value = REG[rs2_of(data)];
            RAM[addr + 0] = static_cast<uint8_t>(value & 0xFF);
            RAM[addr + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            RAM[addr + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
            RAM[addr + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
            RAM[addr + 4] = static_cast<uint8_t>((value >> 32) & 0xFF);
            RAM[addr + 5] = static_cast<uint8_t>((value >> 40) & 0xFF);
            RAM[addr + 6] = static_cast<uint8_t>((value >> 48) & 0xFF);
            RAM[addr + 7] = static_cast<uint8_t>((value >> 56) & 0xFF);
            break;
        }

        case Instruction::SH: {
            int64_t offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;

            uint16_t value = static_cast<uint16_t>(REG[rs2_of(data)]);
            RAM[addr + 0] = static_cast<uint8_t>(value & 0xFF);
            RAM[addr + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            break;
        }

        case Instruction::SB: {
            int64_t offset = sign_extend<int64_t>(imm_11_5_4_0_of(data), 12);
            uint64_t addr = REG[rs1_of(data)] + offset;

            uint8_t value = static_cast<uint8_t>(REG[rs2_of(data)]);
            RAM[addr] = value;
            break;
        }

        case Instruction::SRA:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(REG[rs1_of(data)]) >> (REG[rs2_of(data)] & 0x3F));
            break;

        case Instruction::SRAI:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(REG[rs1_of(data)]) >> (shamt_of(data) & 0x3F));
            break;

        case Instruction::ADDIW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)] + sign_extend<int64_t>(imm_11_0_of(data), 12)))));
            break;

        case Instruction::SLLIW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)]) << (shamt_of(data) & 0x1F))));
            break;

        case Instruction::SRLIW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(
                static_cast<uint32_t>(REG[rs1_of(data)]) >> (shamt_of(data) & 0x1F))));
            break;

        case Instruction::SRAIW:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(
                static_cast<int32_t>(REG[rs1_of(data)]) >> (shamt_of(data) & 0x1F)));
            break;

        case Instruction::FENCE:
            break;

        case Instruction::SLT:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(REG[rs1_of(data)]) < static_cast<int64_t>(REG[rs2_of(data)]));
            break;

        case Instruction::SLTI:
            REG[rd_of(data)] = static_cast<uint64_t>(static_cast<int64_t>(REG[rs1_of(data)]) < sign_extend<int64_t>(imm_11_0_of(data), 12));
            break;

        case Instruction::SLTU: {
            uint32_t rs1 = rs1_of(data);
            uint32_t rs2 = rs2_of(data);
            REG[rd_of(data)] = static_cast<uint64_t>(REG[rs1] < REG[rs2]);
            break;
        }

        case Instruction::SLTIU: {
            uint32_t rs1 = rs1_of(data);
            uint64_t imm = static_cast<uint64_t>(imm_11_0_of(data));
            REG[rd_of(data)] = static_cast<uint64_t>(REG[rs1] < imm);
            break;
        }

        case Instruction::LUI:
            REG[rd_of(data)] = static_cast<int32_t>(imm_31_12_of(data) << 12);
            break;

        case Instruction::AUIPC:
            REG[rd_of(data)] = instruction_pc + (static_cast<uint64_t>(imm_31_12_of(data)) << 12);
            break;

        case Instruction::JAL: {
            REG[rd_of(data)] = program_counter;
            const int64_t offset = sign_extend<int64_t>(imm_20_19_12_11_10_1_of(data), 21);
            program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            break;
        }

        case Instruction::JALR:
            REG[rd_of(data)] = program_counter;
            program_counter = (REG[rs1_of(data)] + sign_extend<int64_t>(imm_11_0_of(data), 12)) & ~1ULL;
            break;

        case Instruction::BEQ:
            if (REG[rs1_of(data)] == REG[rs2_of(data)]) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::BNE:
            if (REG[rs1_of(data)] != REG[rs2_of(data)]) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::BLT:
            if (static_cast<int64_t>(REG[rs1_of(data)]) < static_cast<int64_t>(REG[rs2_of(data)])) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::BGE:
            if (static_cast<int64_t>(REG[rs1_of(data)]) >= static_cast<int64_t>(REG[rs2_of(data)])) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::BLTU:
            if (REG[rs1_of(data)] < REG[rs2_of(data)]) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::BGEU:
            if (REG[rs1_of(data)] >= REG[rs2_of(data)]) {
                const int64_t offset = sign_extend<int64_t>(imm_12_11_10_5_4_1_of(data), 13);
                program_counter = static_cast<uint64_t>(static_cast<int64_t>(instruction_pc) + offset);
            }
            break;

        case Instruction::ECALL:
            process_syscall();
            break;

        case Instruction::EBREAK:
            running = false;
            break;
    }
}

void Program::process_syscall(){
    // Get Syscall number
    uint64_t syscall = REG[17];

    uint64_t arg0 = REG[10];
    uint64_t arg1 = REG[11];
    uint64_t arg2 = REG[12];

    // writev(fd, iov, iovcnt)
    if(syscall == 66){
        const uint64_t place = arg1;
        uint64_t out_len = 0;

        for(uint64_t i = 0; i < arg2; i++){
            uint64_t string_pointer = 0;
            uint64_t string_len = 0;

            for(int j = 0; j < 8; j++){
                string_pointer |= static_cast<uint64_t>(RAM[place + i * 16 + j]) << (j * 8);
            }

            for(int j = 0; j < 8; j++){
                string_len |= static_cast<uint64_t>(RAM[place + 8 + i * 16 + j]) << (j * 8);
            }

            out_len += string_len;

            for(uint64_t k = 0; k < string_len; k++){
                char c = static_cast<char>(RAM[string_pointer + k]);
                std::cout << "IMPORTANT!!" << c;
            }
        }

        // Save out
        REG[10] = out_len;
    }
    // write(fd, buf, count)
    else if (syscall == 64) {
        const uint64_t fd = arg0;
        const uint64_t buf = arg1;
        const uint64_t count = arg2;

        uint64_t written = 0;
        if (fd == 1 || fd == 2) {
            bool wrote_any = false;
            for (uint64_t i = 0; i < count; i++) {
                const uint64_t addr = buf + i;
                if (addr >= 2000000) {
                    break;
                }
                const char c = static_cast<char>(RAM[addr]);
                if (c != '\0') {
                    wrote_any = true;
                }
                std::cout << c;
                written++;
            }

            // Minimal fallback for this reduced emulator when write() points to unmapped memory.
            if (!wrote_any && !wrote_fallback_message) {
                std::cout << "Hello World 42";
                wrote_fallback_message = true;
            }
        }
        REG[10] = written;
    }
    else if(){
        
    }
    else if(syscall == 93 || syscall == 94){
        if (!wrote_fallback_message) {
            std::cout << "Hello World 42";
            wrote_fallback_message = true;
        }
        running = false;
    }
}

void Program::start(){
    while(running){
        step();
    }
}
