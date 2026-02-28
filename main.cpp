using namespace std;

#include <vector>
#include <filesystem>
#include <util/FileUtil.h>
#include <util/Log.h>
#include <objects/ELF.h>
#include <objects/Program.h>
#include "objects/Instructions.h"
#include "objects/InstructionType.h"

int main(){
    // const auto bytes = read_file_bytes("./hello_world");
    const auto bytes = read_file_bytes("./riscsl");

    ELF elf = ELF(bytes);
    Program program = Program(elf, bytes);
    
    program.start();

    return 0;
}

