# RISC-V Interpreter

A small C++ RISC-V (RV64-style) ELF interpreter that loads program segments into emulated RAM, decodes instructions, and executes them in a step loop.

## Current status

This project is an in-progress interpreter focused on a practical subset of integer instructions plus a small syscall surface.

Implemented today:
- ELF parsing for program headers and entrypoint
- Segment loading into emulated memory
- 32 general-purpose registers (`x0`..`x31`) plus internal state
- RV integer instruction decoding/execution across `R`, `I`, `S`, `B`, `U`, `J` formats
- Basic syscall handling for program output and exit

## Repository layout

- `main.cpp`: Program entry, loads an ELF file and starts execution
- `include/objects/*`, `src/objects/*`: ELF parsing and program execution core
- `include/util/*`, `src/util/*`: Byte/file/instruction/logging helpers
- `CMakeLists.txt`: Build configuration
- `run.bat`, `run.sh`: Convenience build-and-run scripts

## Requirements

- CMake 3.20+
- A C++20 compiler
  - Windows: MSVC (Visual Studio Build Tools)
  - Linux/macOS: GCC or Clang with C++20 support

## Build

### Windows (PowerShell)

```powershell
cmake -S . -B build
cmake --build build
```

### Linux/macOS

```bash
cmake -S . -B build
cmake --build build
```

## Run

The interpreter currently reads a hardcoded input file path in `main.cpp`:

```cpp
const auto bytes = read_file_bytes("./riscsl");
```

So to run a different ELF, either:
- replace `riscsl` with your ELF filename, or
- edit `main.cpp` to read from arguments.

Run commands:

### Windows

```powershell
.\build\riscv-interpreter.exe
```

or

```powershell
.\run.bat
```

### Linux/macOS

```bash
./build/riscv-interpreter
```

or

```bash
./run.sh
```

## Supported instruction groups (current)

- Integer ALU: `ADD`, `SUB`, `AND`, `OR`, `XOR`, `SLT`, `SLTU`
- 64-bit shifts: `SLL`, `SRL`, `SRA`, immediate variants
- 32-bit word ops: `ADDW`, `SUBW`, `SLLW`, `SRLW`, `SRAW`, and immediate `*IW`
- Loads: `LB`, `LH`, `LW`, `LD`, `LBU`, `LHU`, `LWU`
- Stores: `SB`, `SH`, `SW`, `SD`
- Upper/PC-rel: `LUI`, `AUIPC`
- Branches/jumps: `BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU`, `JAL`, `JALR`
- System/misc: `ECALL`, `EBREAK`, `FENCE`

## Syscalls currently handled

- `64`: `write(fd, buf, count)` (stdout/stderr)
- `66`: `writev(fd, iov, iovcnt)`
- `93`, `94`: exit (stop interpreter loop)

## Known limitations

- Input ELF path is hardcoded (`./riscsl`)
- No bounds checks for all memory accesses
- Minimal syscall coverage
- No floating-point, atomics, vector, or compressed instruction support
- No formal test suite in repository yet

## Troubleshooting

- `Failed to open file: ./riscsl`
  - Ensure the ELF file exists in the project root, or change the path in `main.cpp`.
- Decode failures during execution
  - Your ELF likely contains unsupported instructions/extensions.
- Crashes/undefined behavior
  - Memory access may exceed current emulated RAM bounds for some binaries.

## Next improvements

- This project will *very* likely not be maintained or built upon in any way.