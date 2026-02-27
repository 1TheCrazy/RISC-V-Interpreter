using namespace std;

#include <vector>
#include <util/FileUtil.h>
#include <util/Log.h>
#include <objects/ELF.h>
#include <objects/Program.h>
#include "objects/Instructions.h"
#include "objects/InstructionType.h"
// REMOVE IN PROD
#include "util/InstructionUtil.h"
// RAM doesn't do anything useful for now
vector<uint8_t> RAM;

int main(){
/*
    const auto bytes = read_file_bytes("./hello_world");
    ELF elf = ELF(bytes);
    Program program = Program(elf, bytes);
*/
    uint32_t inst = 0b00000000101000010000000010010011;
    auto opcode = op_code_of(inst);

    InstructionGroup instr_group = instruction_group_of(opcode);
    InstructionType instr_type = instruction_group_to_type(instr_group);
    Instruction instruction = get_instruction(instr_type, inst);

    std::cout << static_cast<int>(instruction) << std::endl;

    return 0;
}

