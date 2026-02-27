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
    uint32_t inst = 0b01000000011110111000010100110011;
    auto opcode = inst & 0b1111111;
    auto func_3 = (inst >> 11) & 0b111;
    auto func7 = inst >> 24;

    InstructionGroup instr_group = static_cast<InstructionGroup>(opcode);
    InstructionType instr_type = instruction_group_to_type(instr_group);
    Instruction instruction = get_instruction(instr_type, inst);

    return 0;
}

