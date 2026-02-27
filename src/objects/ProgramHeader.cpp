#include <vector>
#include "objects/ProgramHeader.h"
#include "util/ByteUtil.h"

void ProgramHeader::fill(const std::vector<uint8_t>& bytes, uint64_t offset){
    p_type = get_32_by_offset(0x00+offset,bytes);
    p_flags = get_32_by_offset(0x04+offset,bytes);
    p_offset = get_64_by_offset(0x08+offset,bytes);
    p_vaddr = get_64_by_offset(0x10+offset,bytes);
    p_paddr = get_64_by_offset(0x18+offset,bytes);
    p_filesz = get_64_by_offset(0x20+offset,bytes);
    p_memsz = get_64_by_offset(0x28+offset,bytes);
    p_align = get_64_by_offset(0x30+offset,bytes);
}