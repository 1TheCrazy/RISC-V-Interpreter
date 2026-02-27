#include <util/InstructionUtil.h>

Instrcution get_instruction(const InstructionType& instr_type, const uint32_t& instruction){
    switch(instr_type){
        case InstructionType::R : 
            return instruction_from_type_R(instruction);
    }
}

Instrcution instruction_from_type_R(const uint32_t& instruction){
    auto func_3 = (instruction >> 11) & 0b111;
    auto func_7 = instruction >> 24;
    auto rs1 = (instruction >> 14) & 0b11111;
    auto rs2 = (instruction >> 19) & 0b11111;
    auto rd = (instruction >> 7) & 0b11111;

    switch(func_3){
        case 0b000 :
            return func_7 == 0b0000000 ? Instrcution::ADD : Instrcution::SUB;
        case 0b001 :
            return Instrcution::SLL;
        case 0b010 :
            return Instrcution::SLT;
        case 0b011 :
            return Instrcution::SLTU;
        case 0b100: 
            return Instrcution::XOR;
        case 0b101 :
            return func_7 == 0b0000000 ? Instrcution::SRL : Instrcution::SRA;
        case 0b110 : 
            return Instrcution::OR;
        case 0b111 :
            return Instrcution::AND;
    }
}

InstructionType instruction_group_to_type(const InstructionGroup& instr){
    switch(instr){
        case InstructionGroup::LOAD || InstructionGroup::OP_IMM || InstructionGroup::SYSTEM:
            return InstructionType::I;

        case InstructionGroup::AUIPC | InstructionGroup::LUI :
            return InstructionType::U;

        case InstructionGroup::STORE :
            return InstructionType::S;

        case InstructionGroup::OP :
            return InstructionType::R;

        case InstructionGroup::BRANCH :
            return InstructionType::B;

        case InstructionGroup::JAL :
            return InstructionType::J;
    };
}