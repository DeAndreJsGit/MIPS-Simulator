# MIPS Pipeline Simulator

## CS3339 Fall 2025 Course Project

A 5-stage pipelined MIPS processor simulator written in C++. The simulator executes MIPS assembly programs instruction-by-instruction and displays the final machine state (registers and memory).

## Features

- **5-Stage Pipeline**: Faithful implementation of IF, ID, EX, MEM, WB stages
- **Pipeline Registers**: IF/ID, ID/EX, EX/MEM, MEM/WB state registers
- **Control Signals**: Proper generation of control signals during decode
- **Debug Mode**: Step-by-step pipeline visualization
- **Binary Representation**: Shows binary encoding of instructions
- **Error Handling**: Detailed error messages with error file generation

## Supported Instructions

| Opcode | Description |
|--------|-------------|
| ADD    | Signed integer addition (R-type) |
| ADDI   | Add immediate (I-type) |
| SUB    | Signed integer subtraction (R-type) |
| MUL    | Integer multiplication (R-type) |
| AND    | Bitwise AND (R-type) |
| OR     | Bitwise OR (R-type) |
| SLL    | Shift left logical |
| SRL    | Shift right logical |
| LW     | Load word from memory |
| SW     | Store word to memory |
| BEQ    | Branch if equal |
| J      | Unconditional jump |
| NOP    | No operation |

## Building

### Requirements
- C++17 compatible compiler (g++ recommended)
- Make

### Compile
```bash
make
```

### Debug Build
```bash
make debug
```

### Clean
```bash
make clean
```

## Usage

### Basic Execution
```bash
./mips_sim <input.asm>
```

### With Debug Output
```bash
./mips_sim <input.asm> --debug
```
or
```bash
./mips_sim <input.asm> -d
```

### Help
```bash
./mips_sim --help
```

## Input Format

The simulator accepts MIPS assembly files with the following conventions:

- Comments start with `#`
- Labels end with `:`
- Register names: `$0`-`$31` or named (`$t0`, `$s0`, `$zero`, etc.)
- Memory operands: `offset($base)` format

### Example Program
```assembly
# Simple arithmetic test
        ADDI $t0, $zero, 10    # t0 = 10
        ADDI $t1, $zero, 5     # t1 = 5
        ADD  $t2, $t0, $t1     # t2 = t0 + t1 = 15
        SUB  $t3, $t0, $t1     # t3 = t0 - t1 = 5
        SW   $t2, 0($zero)     # mem[0] = 15
        LW   $t4, 0($zero)     # t4 = mem[0] = 15
```

## Output

### Normal Mode
Displays:
- Number of instructions loaded
- Binary representation of instructions
- Final register file contents (non-zero values)
- Final memory contents (non-zero values)
- Total cycle count

### Debug Mode
Additionally displays after each cycle:
- Current cycle number
- Program counter (PC)
- Contents of all pipeline registers (IF/ID, ID/EX, EX/MEM, MEM/WB)
- Control signals for each stage
- Register file snapshot

## Project Structure

```
mips_simulator/
├── main.cpp      # Entry point, argument parsing
├── parser.cpp    # Assembly file parsing
├── parser.h
├── cpu.cpp       # CPU simulation logic
├── cpu.h         # Data structures (Instruction, Pipeline registers)
├── stages.cpp    # Pipeline stage implementations
├── stages.h
├── debug.cpp     # Debug output formatting
├── debug.h
├── errors.cpp    # Error handling
├── errors.h
├── Makefile      # Build configuration
└── README.md     # This file
```

## Design Principles

The simulator follows **Single Responsibility Principle (SRP)**:

- **Parser**: Handles all assembly parsing and label resolution
- **CPU**: Manages architectural state and simulation loop
- **Stages**: Implements individual pipeline stage logic
- **Debug**: Handles all output formatting
- **Errors**: Manages error collection and reporting

## Error Handling

When parsing errors occur:
1. Errors are printed to stderr
2. An `.err` file is generated with detailed error information
3. Simulation does not proceed

Common errors detected:
- Unknown instructions
- Invalid register names
- Undefined labels
- Malformed operands
- Invalid immediate values

## Notes

- Input programs are assumed to be **free of data and control hazards**
- Memory is word-addressed (4 bytes per word)
- Memory size: 1024 words (4KB)
- All registers initialize to 0
- `$zero` is always kept at 0

## Authors

CS3339 Fall 2025 Team Project

## License

For educational purposes only.
