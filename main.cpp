using namespace std;

#include <vector>
#include <util/FileUtil.h>
#include <util/Log.h>
#include <objects/ELF.h>

// RAM doesn't do anything useful for now
vector<uint8_t> RAM;

int main(){
    const auto bytes = read_file_bytes("./hello_world.elf");
    ELF elf = ELF(bytes);

    // Use the ELF...

    return 0;
}

