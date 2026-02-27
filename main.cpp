using namespace std;

#include <vector>
#include <util/FileUtil.h>
#include <util/Log.h>
#include <objects/ELF.h>
#include <objects/Program.h>

// RAM doesn't do anything useful for now
vector<uint8_t> RAM;

int main(){
    const auto bytes = read_file_bytes("./hello_world");
    ELF elf = ELF(bytes);
    Program program = Program(elf, bytes);

    return 0;
}

