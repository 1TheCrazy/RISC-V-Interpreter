using namespace std;

#include <vector>
#include <objects/ELFHeader.h>
#include <util/ByteUtil.h>

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

void ELFHeader::fill(const std::vector<uint8_t>& bytes){
    e_entry = get_e_entry(bytes);
    e_phoff = get_e_phoff(bytes);
    is_little_endian = is_elf_little_endian(bytes);
    e_phentsize = get_16_by_offset(0x36, bytes);
    e_phenum = get_16_by_offset(0x38, bytes);
}