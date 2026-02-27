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
    // Load Group - Type I
    LB,
    LH,
    LW,
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
    // SYSTEM - Type I
    ECALL,
    EBREAK,
    // JALR - Type I
    JALR,
    // STORE - Type S
    SB,
    SH,
    SW,
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