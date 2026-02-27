#include <objects/Program.h>
#include <objects/Instructions.h>
#include <objects/InstructionType.h>
#include <vector>
#include <iostream>

Instruction get_instruction(const InstructionType& instr_type, const uint32_t& instruction);
Instruction instruction_from_type_R(const uint32_t& instruction);
InstructionType instruction_group_to_type(const InstructionGroup& instr);