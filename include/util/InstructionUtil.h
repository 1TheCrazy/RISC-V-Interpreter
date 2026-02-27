#include <objects/Program.h>
#include <objects/Instructions.h>
#include <objects/InstructionType.h>
#include <vector>
#include <iostream>

Instruction get_instruction(const InstructionType& instr_type, const uint32_t& instruction);
Instruction instruction_from_type_I(const uint32_t& instruction);
Instruction instruction_from_type_S(const uint32_t& instruction);
Instruction instruction_from_type_U(const uint32_t& instruction);
Instruction instruction_from_type_B(const uint32_t& instruction);
Instruction instruction_from_type_J(const uint32_t& instruction);
Instruction instruction_from_type_R(const uint32_t& instruction);
InstructionType instruction_group_to_type(const InstructionGroup& instr);
uint32_t rd_of(const uint32_t& instruction);
uint32_t rs1_of(const uint32_t& instruction);
uint32_t rs2_of(const uint32_t& instruction);
uint32_t imm_11_5_4_0_of(const uint32_t& instruction);
uint32_t imm_11_0_of(const uint32_t& instruction);
uint32_t imm_12_11_10_5_4_1_of(const uint32_t& instruction);
uint32_t imm_20_19_12_11_10_1_of(const uint32_t& instruction);
uint32_t imm_31_12_of(const uint32_t& instruction);
uint32_t func_3_of(const uint32_t& instruction);
uint32_t func_7_of(const uint32_t& instruction);
uint32_t op_code_of(const uint32_t& instruction);
InstructionGroup instruction_group_of(const uint32_t& instruction);
uint32_t shamt_of(const uint32_t& instruction);