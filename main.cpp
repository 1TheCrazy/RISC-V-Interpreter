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

    for(int i = 0; i < elf.program_headers.size(); i++){
        auto header = elf.program_headers[i];

        log("MemSize: " + to_string(header.p_memsz));
        log("VirtualAddr: " + to_string(header.p_vaddr));

    }

    log("Found " + to_string(elf.program_headers.size()) + " Headers");

    return 0;
}

