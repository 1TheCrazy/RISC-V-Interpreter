#include <util/InstructionUtil.h>
#include <stdexcept>

Instruction get_instruction(const InstructionType& instr_type, const uint32_t& instruction){
    switch(instr_type){
        case InstructionType::R : 
            return instruction_from_type_R(instruction);
        case InstructionType::I :
            return instruction_from_type_I(instruction);
        case InstructionType::S :
            return instruction_from_type_S(instruction);
        case InstructionType::U :
            return instruction_from_type_U(instruction);
        case InstructionType::B :
            return instruction_from_type_B(instruction);
        case InstructionType::J :
            return instruction_from_type_J(instruction);
    }
    throw std::runtime_error("Unsupported instruction type");
}

Instruction instruction_from_type_J(const uint32_t& instruction){
    return Instruction::JAL;
}

Instruction instruction_from_type_B(const uint32_t& instruction){
    auto func_3 = func_3_of(instruction);

    switch(func_3){
        case 0b000:
            return Instruction::BEQ;
        case 0b001:
            return Instruction::BNE;
        case 0b100:
            return Instruction::BLT;
        case 0b101:
            return Instruction::BGE;
        case 0b110:
            return Instruction::BLTU;
        case 0b111:
            return Instruction::BGEU;
    }
    throw std::runtime_error("Unsupported B-type instruction");
}

Instruction instruction_from_type_U(const uint32_t& instruction){
    auto opcode = op_code_of(instruction);
    InstructionGroup instr_group = instruction_group_of(opcode);

    return instr_group == InstructionGroup::LUI ? Instruction::LUI : Instruction::AUIPC;
}

Instruction instruction_from_type_S(const uint32_t& instruction){
    auto func_3 = func_3_of(instruction);

    switch(func_3){
        case 0b000:
            return Instruction::SB;
        case 0b001:
            return Instruction::SH;
        case 0b010:
            return Instruction::SW;
        case 0b011:
            return Instruction::SD;
    }
    throw std::runtime_error("Unsupported S-type instruction");
}

Instruction instruction_from_type_I(const uint32_t& instruction){
    auto func_3 = func_3_of(instruction);
    auto imm_11_0 = imm_11_0_of(instruction);

    auto opcode = op_code_of(instruction);
    InstructionGroup instr_group = instruction_group_of(opcode);

    switch(instr_group){
        case InstructionGroup::LOAD :
            switch(func_3){
                case 0b000:
                    return Instruction::LB;
                case 0b001:
                    return Instruction::LH;
                case 0b010:
                    return Instruction::LW;
                case 0b011:
                    return Instruction::LD;
                case 0b100:
                    return Instruction::LBU;
                case 0b101:
                    return Instruction::LHU;
                case 0b110:
                    return Instruction::LWU;
            }
            break;
        
        case InstructionGroup::OP_IMM :
            switch(func_3){
                case 0b000:
                    return Instruction::ADDI;
                case 0b010:
                    return Instruction::SLTI;
                case 0b011:
                    return Instruction::SLTIU;
                case 0b100:
                    return Instruction::XORI;
                case 0b110:
                    return Instruction::ORI;
                case 0b111:
                    return Instruction::ANDI;
                case 0b001:
                    return Instruction::SLLI;
                case 0b101: {
                    const auto funct7 = func_7_of(instruction);
                    return funct7 == 0b0100000 ? Instruction::SRAI : Instruction::SRLI;
                }
            }
            break;

        case InstructionGroup::OP_IMM_32 :
            switch(func_3){
                case 0b000:
                    return Instruction::ADDIW;
                case 0b001:
                    return Instruction::SLLIW;
                case 0b101: {
                    const auto funct7 = func_7_of(instruction);
                    return funct7 == 0b0100000 ? Instruction::SRAIW : Instruction::SRLIW;
                }
            }
            break;
        
        case InstructionGroup::SYSTEM :
            return imm_11_0 == 0b000000000000 ? Instruction::ECALL : Instruction::EBREAK;

        case InstructionGroup::JALR:
            return Instruction::JALR;

        case InstructionGroup::MISC_MEM:
            return Instruction::FENCE;
    }
    throw std::runtime_error("Unsupported I-type instruction");
}

Instruction instruction_from_type_R(const uint32_t& instruction){
    auto func_3 = func_3_of(instruction);
    auto func_7 = func_7_of(instruction);
    auto opcode = op_code_of(instruction);
    InstructionGroup instr_group = instruction_group_of(opcode);

    if (instr_group == InstructionGroup::OP_32) {
        switch(func_3){
            case 0b000:
                return func_7 == 0b0000000 ? Instruction::ADDW : Instruction::SUBW;
            case 0b001:
                return Instruction::SLLW;
            case 0b101:
                return func_7 == 0b0000000 ? Instruction::SRLW : Instruction::SRAW;
        }
        throw std::runtime_error("Unsupported OP_32 instruction");
    }

    switch(func_3){
        case 0b000 :
            return func_7 == 0b0000000 ? Instruction::ADD : Instruction::SUB;
        case 0b001 :
            return Instruction::SLL;
        case 0b010 :
            return Instruction::SLT;
        case 0b011 :
            return Instruction::SLTU;
        case 0b100: 
            return Instruction::XOR;
        case 0b101 :
            return func_7 == 0b0000000 ? Instruction::SRL : Instruction::SRA;
        case 0b110 : 
            return Instruction::OR;
        case 0b111 :
            return Instruction::AND;
    }
    throw std::runtime_error("Unsupported R-type instruction");
}

InstructionType instruction_group_to_type(const InstructionGroup& instr){
    switch(instr){
        case InstructionGroup::LOAD :
        case InstructionGroup::OP_IMM : 
        case InstructionGroup::OP_IMM_32 :
        case InstructionGroup::SYSTEM :
        case InstructionGroup::MISC_MEM :
        case InstructionGroup::JALR:
            return InstructionType::I;

        case InstructionGroup::AUIPC :
        case InstructionGroup::LUI :
            return InstructionType::U;

        case InstructionGroup::STORE :
            return InstructionType::S;

        case InstructionGroup::OP :
        case InstructionGroup::OP_32 :
            return InstructionType::R;

        case InstructionGroup::BRANCH :
            return InstructionType::B;

        case InstructionGroup::JAL :
            return InstructionType::J;
    };
    throw std::runtime_error("Unsupported instruction group");
}

uint32_t rd_of(const uint32_t& instruction){
    uint32_t rd = (instruction >> 7) & 0b11111;
    return rd;
}

uint32_t rs1_of(const uint32_t& instruction){
    uint32_t rs1 = (instruction >> 15) & 0b11111;
    return rs1;
}

uint32_t rs2_of(const uint32_t& instruction){
    uint32_t rs2 = (instruction >> 20) & 0b11111;
    return rs2;
}

uint32_t imm_11_5_4_0_of(const uint32_t& instruction){
    uint32_t imm_11_5 = (instruction >> 25) & 0b1111111;
    uint32_t imm_4_0 = (instruction >> 7) & 0b11111;
    uint32_t imm_11_5_4_0 = imm_11_5 << 5 | imm_4_0;
    return imm_11_5_4_0;
}

uint32_t imm_11_0_of(const uint32_t& instruction){
    uint32_t imm_11_0 = (instruction >> 20) & 0xFFF;
    return imm_11_0;
}

uint32_t imm_31_12_of(const uint32_t& instruction){
    uint32_t imm_31_12 = instruction >> 12;
    return imm_31_12;
}

uint32_t imm_12_11_10_5_4_1_of(const uint32_t& instruction){
    uint32_t imm_11 = (instruction >> 7) & 0x1;
    uint32_t imm_4_1 = (instruction >> 8) & 0xF;
    uint32_t imm_10_5 = (instruction >> 25) & 0x3F;
    uint32_t imm_12 = (instruction >> 31) & 0x1;
    uint32_t imm_12_11_10_5_4_1 =
      (imm_12   << 12)
    | (imm_11   << 11)
    | (imm_10_5 << 5)
    | (imm_4_1  << 1);

    return imm_12_11_10_5_4_1;
}

uint32_t imm_20_19_12_11_10_1_of(const uint32_t& instruction){
    uint32_t imm_20 = (instruction >> 31) & 0x1;
    uint32_t imm_19_12 = (instruction >> 12) & 0xFF;
    uint32_t imm_11 = (instruction >> 20) & 0x1;
    uint32_t imm_10_1 = (instruction >> 21) & 0x3FF;
    uint32_t imm_20_19_12_11_10_1 =
      (imm_20    << 20)
    | (imm_19_12 << 12)
    | (imm_11    << 11)
    | (imm_10_1  << 1);
    return imm_20_19_12_11_10_1;
}

uint32_t func_3_of(const uint32_t& instruction){
    uint32_t func_3 = (instruction >> 12) & 0b111;
    return func_3;
}

uint32_t func_7_of(const uint32_t& instruction){
    uint32_t func_7 = (instruction >> 25) & 0b1111111;
    return func_7;
}

uint32_t op_code_of(const uint32_t& instruction){
    uint32_t opcode = instruction & 0b1111111;
    return opcode;
}

InstructionGroup instruction_group_of(const uint32_t& opcode){
    return static_cast<InstructionGroup>(opcode);
}

uint32_t shamt_of(const uint32_t& instruction) {
    uint32_t shamt = (instruction >> 20) & 0b111111;
    return shamt;
}
