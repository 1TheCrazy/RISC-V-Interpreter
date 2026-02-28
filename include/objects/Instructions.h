#pragma once

enum class Instruction{
    // OP - Type R
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    ADDW,
    SUBW,
    SLLW,
    SRLW,
    SRAW,
    // Load Group - Type I
    LB,
    LH,
    LW,
    LD,
    LWU,
    LBU,
    LHU,
    // OP_IMM - Type I
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADDIW,
    SLLIW,
    SRLIW,
    SRAIW,
    FENCE,
    // SYSTEM - Type I
    ECALL,
    EBREAK,
    // JALR - Type I
    JALR,
    // STORE - Type S
    SB,
    SH,
    SW,
    SD,
    // AUIPC - Type U
    AUIPC,
    // LUI - Type U
    LUI,
    // BRANCH - Type B
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    // JAL - Type J
    JAL
};
