#include <iostream>
using namespace std;

#include <cstdint>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>

uint64_t get_64_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint64_t number = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 8; i++){
        number |= uint64_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
   
    return number;
}

uint32_t get_32_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint32_t number = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 4; i++){
        number |= uint32_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
    
    return number;
}

uint16_t get_16_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint16_t number = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 2; i++){
        number |= uint16_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
    
    return number;
}

// Util for reading a file in bytes
std::vector<std::uint8_t> read_file_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Failed to open file: " + path);

    f.seekg(0, std::ios::end);
    const std::streamsize size = f.tellg();
    if (size < 0) throw std::runtime_error("tellg failed");

    std::vector<std::uint8_t> data(static_cast<size_t>(size));

    f.seekg(0, std::ios::beg);
    if (!f.read(reinterpret_cast<char*>(data.data()), size))
        throw std::runtime_error("Failed to read file: " + path);

    return data;
}

uint64_t get_e_entry(const std::vector<uint8_t>& bytes){
    // Skip to e_entry as per spec
    int pointer = 0x18;
    uint64_t e_entry = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 8; i++){
        e_entry |= uint64_t(bytes[pointer]) << i * 8;
        pointer++;    
    }

    return e_entry;
}

bool is_little_endian(const std::vector<uint8_t>& bytes){
    // Skip to endian entry as per spec
    int pointer = 0x05;

    // Read 8 Byte entry point
    uint8_t endianess = bytes[pointer];

    return endianess == 1 ? true : false;
}

struct ProgramHeader{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

ProgramHeader get_program_headers(const std::vector<uint8_t>& bytes, uint64_t offset){
    ProgramHeader p;

    p.p_type = get_32_by_offset(0x00+offset,bytes);
    p.p_flags = get_32_by_offset(0x04+offset,bytes);
    p.p_offset = get_64_by_offset(0x08+offset,bytes);
    p.p_vaddr = get_64_by_offset(0x10+offset,bytes);
    p.p_paddr = get_64_by_offset(0x18+offset,bytes);
    p.p_filesz = get_64_by_offset(0x20+offset,bytes);
    p.p_memsz = get_64_by_offset(0x28+offset,bytes);
    p.p_align = get_64_by_offset(0x30+offset,bytes);

    return p; 
}

struct ELFHeader{
    bool is_64;
    bool is_little_endian;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint16_t e_phentsize;
    uint16_t e_phenum;
};

uint64_t get_e_phoff(const std::vector<uint8_t>& bytes){
    // Skip to e_entry as per spec
    long pointer = 0x20;
    uint64_t e_phoff = 0;

    // Read 8 Byte entry point
    for(int i = 0; i < 8; i++){
        e_phoff |= uint64_t(bytes[pointer]) << i * 8;
        pointer++;    
    }

    return e_phoff;
}

class ELF{
    public:
        vector<ProgramHeader> program_headers;
        ELFHeader elf_header;

        ELF(const std::vector<uint8_t>& bytes){
            elf_header.e_entry = get_e_entry(bytes);
            elf_header.e_phoff = get_e_phoff(bytes);
            elf_header.is_little_endian = is_little_endian(bytes);
            elf_header.e_phentsize = get_16_by_offset(0x36, bytes);
            elf_header.e_phenum = get_16_by_offset(0x38, bytes);

           // ProgramHeader p = get_program_headers(bytes, elf_header.e_phoff);
           set_program_headers(bytes);
        }
        

    private:
        void set_program_headers(const std::vector<uint8_t>& bytes){
            for(int i = 0; i < elf_header.e_phenum; i++){
                ProgramHeader p = get_program_headers(bytes,elf_header.e_phoff+(i+1)*elf_header.e_phentsize);
                program_headers.push_back(p);
            }
        }
};

vector<uint8_t> RAM;

void log(string msg) {
    cout << msg << '\n';
}

int main(){
    const auto bytes = read_file_bytes("./hello_world.elf");
    ELF elf = ELF(bytes);

    log(to_string(elf.elf_header.e_phenum));
    cout << elf.program_headers.size() << '\n';
    for(int i=0; i< elf.program_headers.size(); i++){
        log(to_string(elf.program_headers[i].p_type));
    }
    

    return 0;
}

