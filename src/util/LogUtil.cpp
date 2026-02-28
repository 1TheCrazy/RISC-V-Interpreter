#include <util/LogUtil.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <util/InstructionUtil.h>

namespace {
std::string utc_timestamp_now() {
    const std::time_t now = std::time(nullptr);
    std::tm utc_time{};
#ifdef _WIN32
    gmtime_s(&utc_time, &now);
#else
    gmtime_r(&now, &utc_time);
#endif

    std::ostringstream out;
    out << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

std::string hex_u64(uint64_t value) {
    std::ostringstream out;
    out << "0x" << std::hex << std::nouppercase << value;
    return out.str();
}
}  // namespace

std::string instruction_to_string(Instruction instruction) {
    switch (instruction) {
        case Instruction::ADD: return "ADD";
        case Instruction::SUB: return "SUB";
        case Instruction::SLL: return "SLL";
        case Instruction::SLT: return "SLT";
        case Instruction::SLTU: return "SLTU";
        case Instruction::XOR: return "XOR";
        case Instruction::SRL: return "SRL";
        case Instruction::SRA: return "SRA";
        case Instruction::OR: return "OR";
        case Instruction::AND: return "AND";
        case Instruction::ADDW: return "ADDW";
        case Instruction::SUBW: return "SUBW";
        case Instruction::SLLW: return "SLLW";
        case Instruction::SRLW: return "SRLW";
        case Instruction::SRAW: return "SRAW";
        case Instruction::LB: return "LB";
        case Instruction::LH: return "LH";
        case Instruction::LW: return "LW";
        case Instruction::LD: return "LD";
        case Instruction::LWU: return "LWU";
        case Instruction::LBU: return "LBU";
        case Instruction::LHU: return "LHU";
        case Instruction::ADDI: return "ADDI";
        case Instruction::SLTI: return "SLTI";
        case Instruction::SLTIU: return "SLTIU";
        case Instruction::XORI: return "XORI";
        case Instruction::ORI: return "ORI";
        case Instruction::ANDI: return "ANDI";
        case Instruction::SLLI: return "SLLI";
        case Instruction::SRLI: return "SRLI";
        case Instruction::SRAI: return "SRAI";
        case Instruction::ADDIW: return "ADDIW";
        case Instruction::SLLIW: return "SLLIW";
        case Instruction::SRLIW: return "SRLIW";
        case Instruction::SRAIW: return "SRAIW";
        case Instruction::FENCE: return "FENCE";
        case Instruction::ECALL: return "ECALL";
        case Instruction::EBREAK: return "EBREAK";
        case Instruction::JALR: return "JALR";
        case Instruction::SB: return "SB";
        case Instruction::SH: return "SH";
        case Instruction::SW: return "SW";
        case Instruction::SD: return "SD";
        case Instruction::AUIPC: return "AUIPC";
        case Instruction::LUI: return "LUI";
        case Instruction::BEQ: return "BEQ";
        case Instruction::BNE: return "BNE";
        case Instruction::BLT: return "BLT";
        case Instruction::BGE: return "BGE";
        case Instruction::BLTU: return "BLTU";
        case Instruction::BGEU: return "BGEU";
        case Instruction::JAL: return "JAL";
    }
    return "UNKNOWN";
}

std::string instruction_type_to_string(Instruction instruction) {
    switch (instruction) {
        case Instruction::ADD:
        case Instruction::SUB:
        case Instruction::SLL:
        case Instruction::SLT:
        case Instruction::SLTU:
        case Instruction::XOR:
        case Instruction::SRL:
        case Instruction::SRA:
        case Instruction::OR:
        case Instruction::AND:
        case Instruction::ADDW:
        case Instruction::SUBW:
        case Instruction::SLLW:
        case Instruction::SRLW:
        case Instruction::SRAW:
            return "RType";

        case Instruction::LB:
        case Instruction::LH:
        case Instruction::LW:
        case Instruction::LD:
        case Instruction::LWU:
        case Instruction::LBU:
        case Instruction::LHU:
        case Instruction::ADDI:
        case Instruction::SLTI:
        case Instruction::SLTIU:
        case Instruction::XORI:
        case Instruction::ORI:
        case Instruction::ANDI:
        case Instruction::SLLI:
        case Instruction::SRLI:
        case Instruction::SRAI:
        case Instruction::ADDIW:
        case Instruction::SLLIW:
        case Instruction::SRLIW:
        case Instruction::SRAIW:
        case Instruction::FENCE:
        case Instruction::ECALL:
        case Instruction::EBREAK:
        case Instruction::JALR:
            return "IType";

        case Instruction::SB:
        case Instruction::SH:
        case Instruction::SW:
        case Instruction::SD:
            return "SType";

        case Instruction::AUIPC:
        case Instruction::LUI:
            return "UType";

        case Instruction::BEQ:
        case Instruction::BNE:
        case Instruction::BLT:
        case Instruction::BGE:
        case Instruction::BLTU:
        case Instruction::BGEU:
            return "BType";

        case Instruction::JAL:
            return "JType";
    }
    return "UnknownType";
}

std::string register_name(uint32_t index) {
    static const char* kRegNames[32] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    if (index < 32) {
        return kRegNames[index];
    }
    return "x?";
}

std::string register_label(uint32_t index) {
    std::ostringstream out;
    out << register_name(index) << "(x" << index << ")";
    return out.str();
}

std::vector<RegisterUsage> instruction_register_usages(Instruction instruction, uint32_t data) {
    switch (instruction) {
        case Instruction::ADD:
        case Instruction::SUB:
        case Instruction::SLL:
        case Instruction::SLT:
        case Instruction::SLTU:
        case Instruction::XOR:
        case Instruction::SRL:
        case Instruction::SRA:
        case Instruction::OR:
        case Instruction::AND:
        case Instruction::ADDW:
        case Instruction::SUBW:
        case Instruction::SLLW:
        case Instruction::SRLW:
        case Instruction::SRAW:
            return {
                {"rd", rd_of(data)},
                {"rs1", rs1_of(data)},
                {"rs2", rs2_of(data)}
            };

        case Instruction::FENCE:
            return {};

        case Instruction::LB:
        case Instruction::LH:
        case Instruction::LW:
        case Instruction::LD:
        case Instruction::LWU:
        case Instruction::LBU:
        case Instruction::LHU:
        case Instruction::ADDI:
        case Instruction::SLTI:
        case Instruction::SLTIU:
        case Instruction::XORI:
        case Instruction::ORI:
        case Instruction::ANDI:
        case Instruction::SLLI:
        case Instruction::SRLI:
        case Instruction::SRAI:
        case Instruction::ADDIW:
        case Instruction::SLLIW:
        case Instruction::SRLIW:
        case Instruction::SRAIW:
        case Instruction::JALR:
            return {
                {"rd", rd_of(data)},
                {"rs1", rs1_of(data)}
            };

        case Instruction::SB:
        case Instruction::SH:
        case Instruction::SW:
        case Instruction::SD:
        case Instruction::BEQ:
        case Instruction::BNE:
        case Instruction::BLT:
        case Instruction::BGE:
        case Instruction::BLTU:
        case Instruction::BGEU:
            return {
                {"rs1", rs1_of(data)},
                {"rs2", rs2_of(data)}
            };

        case Instruction::LUI:
        case Instruction::AUIPC:
        case Instruction::JAL:
            return {
                {"rd", rd_of(data)}
            };

        case Instruction::ECALL:
            return {
                {"a7", 17},
                {"a0", 10},
                {"a1", 11},
                {"a2", 12}
            };

        case Instruction::EBREAK:
            return {};
    }
    return {};
}

void log_debug_instruction_state(Instruction instruction, uint32_t data, uint64_t pc, const uint64_t* regs) {
    const auto usages = instruction_register_usages(instruction, data);
    const std::string prefix = "[" + utc_timestamp_now() + " DEBUG RISCV::system] ";

    std::ostringstream header;
    header << prefix << instruction_to_string(instruction) << " " << instruction_type_to_string(instruction) << " (";
    for (std::size_t i = 0; i < usages.size(); ++i) {
        if (i > 0) {
            header << ", ";
        }
        header << usages[i].role << ": " << register_label(usages[i].index);
    }
    header << ")";
    std::cout << header.str() << '\n';

    std::cout << prefix << "PC: " << hex_u64(pc) << '\n';

    for (uint32_t i = 0; i < 32; ++i) {
        if (regs[i] == 0) {
            continue;
        }
        std::cout << "    " << register_label(i) << ": " << hex_u64(regs[i]) << '\n';
    }
}
