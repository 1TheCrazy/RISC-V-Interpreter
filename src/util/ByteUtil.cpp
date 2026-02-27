#include "util/ByteUtil.h"

uint64_t get_64_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint64_t number = 0;

    for(int i = 0; i < 8; i++){
        number |= uint64_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
   
    return number;
}

uint32_t get_32_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint32_t number = 0;

    for(int i = 0; i < 4; i++){
        number |= uint32_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
    
    return number;
}

uint16_t get_16_by_offset(int pointer, const std::vector<uint8_t>& bytes){
    uint16_t number = 0;

    for(int i = 0; i < 2; i++){
        number |= uint16_t(bytes[pointer]) << i * 8;
        pointer++;    
    }
    
    return number;
}

bool is_elf_little_endian(const std::vector<uint8_t>& bytes){
    // Skip to endian entry as per spec
    int pointer = 0x05;

    // Read 8 Byte entry point
    uint8_t endianess = bytes[pointer];

    return endianess == 1 ? true : false;
}