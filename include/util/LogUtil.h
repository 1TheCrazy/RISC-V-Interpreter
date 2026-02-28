#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <objects/Instructions.h>

struct RegisterUsage {
    const char* role;
    uint32_t index;
};

std::string instruction_to_string(Instruction instruction);
std::string instruction_type_to_string(Instruction instruction);
std::string register_name(uint32_t index);
std::string register_label(uint32_t index);
std::vector<RegisterUsage> instruction_register_usages(Instruction instruction, uint32_t data);

void log_debug_instruction_state(Instruction instruction, uint32_t data, uint64_t pc, const uint64_t* regs);
