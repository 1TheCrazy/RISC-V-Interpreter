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
    std::cout << "Hel2lo" <<'\n'; 
    const auto bytes = read_file_bytes("./hello_world");
    ELF elf = ELF(bytes);
    Program program = Program(elf, bytes);
    program.start();

    std::cout << "Should be done." << std::endl;

    return 0;
}

