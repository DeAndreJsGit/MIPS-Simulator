#include "../include/cpu.h"
#include "stages.h"
#include "debug.h"
#include <iostream>
#include <iomanip>

using namespace std;

string opcodeToString(Opcode op) {
    switch (op) {
        case Opcode::ADD:  return "ADD";
        case Opcode::ADDI: return "ADDI";
        case Opcode::SUB:  return "SUB";
        case Opcode::MUL:  return "MUL";
        case Opcode::AND:  return "AND";
        case Opcode::OR:   return "OR";
        case Opcode::SLL:  return "SLL";
        case Opcode::SRL:  return "SRL";
        case Opcode::LW:   return "LW";
        case Opcode::SW:   return "SW";
        case Opcode::BEQ:  return "BEQ";
        case Opcode::J:    return "J";
        case Opcode::NOP:  return "NOP";
        default:           return "UNKNOWN";
    }
}

// Step 9: CPU is constructed for main.cpp after successful parsing of assembly file
// It sets the program counter pc to 0 so we start at the first instruction.
// It sets cycleCount to 0.
// It stores whether debug mode is on or off.
// It resets all 32 registers to 0.
// It allocates 1024 words of memory, all initialized to 0.
// It clears all four pipeline registers (IF_ID, ID_EX, EX_MEM, MEM_WB) so the pipeline starts empty.
// This simulates 
CPU::CPU(const Program& prog, bool debug)
    : instructions(prog.instructions)
    , pc(0)
    , cycleCount(0)
    , debugMode(debug)
{
    registers.fill(0);
    memory.resize(1024, 0);

    if_id = IF_ID();
    id_ex = ID_EX();
    ex_mem = EX_MEM();
    mem_wb = MEM_WB();
}

// Step 10: CPU helper functions that delegate to PipelineStages
// generateControl and executeALU forward the work to PipelineStages in stages.cpp
ControlSignals CPU::generateControl(const Instruction& instr) {
    return PipelineStages::generateControl(instr);
}

int32_t CPU::executeALU(const Instruction& instr, int32_t op1, int32_t op2) {
    return PipelineStages::executeALU(instr, op1, op2);
}

bool CPU::pipelineEmpty() const {
    return !if_id.valid && !id_ex.valid && !ex_mem.valid && !mem_wb.valid;
}

// Step 11: stepPipeline executes one cycle of the pipeline
void CPU::stepPipeline() {
    // Next cycle values
    IF_ID next_if_id;
    ID_EX next_id_ex;
    EX_MEM next_ex_mem;
    MEM_WB next_mem_wb;

    // // If the instruction needs to write to a register (like an ADD or LW), 
    // WB takes the ALU result or memory data and writes it into the register file.
    PipelineStages::wbStage(mem_wb, registers);

    // If the instruction needs to read or write memory (like LW or SW),
    // it (LW) reads from memory or (SW) writes to memory through
    next_mem_wb = PipelineStages::memStage(ex_mem, memory);

    // EX Stage performs arthmetic/logical operations
    // Computes the address for load/store instructions
    // Checks (BEQ) branch and (J) jumps whether to change the PC or not 
    bool branchTaken = false;
    size_t branchTarget = 0;
    next_ex_mem = PipelineStages::exStage(id_ex, branchTaken, branchTarget);

    // Decode stage
    next_id_ex = PipelineStages::idStage(if_id, registers);

    // If the PC is still within the program ranges
    if (pc < instructions.size()) {
        next_if_id.valid = true;
        next_if_id.pc = pc;
        next_if_id.instr = instructions[pc];
        pc++;
    }

    // === Branch / Jump handling ===
    if (branchTaken) {
        next_if_id = IF_ID();   // flush
        next_id_ex = ID_EX();   // flush
        pc = branchTarget;      // redirect PC
    }

    // Update pipeline registers
    mem_wb = next_mem_wb;
    ex_mem = next_ex_mem;
    id_ex = next_id_ex;
    if_id = next_if_id;

    // Enforce $zero = 0
    registers[0] = 0;
}
// Step 12: Main simulation loop that runs until all instructions complete
// Shows each instruction’s binary + assembly (and debug info if enabled)
void CPU::run() {
    cout << "\n=== STARTING SIMULATION ===" << endl;

    Debug::printBinaryRepresentation(instructions);

    const size_t MAX_CYCLES = 10000;

    while ((pc < instructions.size() || !pipelineEmpty()) && cycleCount < MAX_CYCLES) {
        cycleCount++;
        stepPipeline();

        if (debugMode) {
            Debug::printPipelineState(*this);
        }
    }

    if (cycleCount >= MAX_CYCLES) {
        cerr << "\nWarning: Simulation stopped after " << MAX_CYCLES
             << " cycles" << endl;
    }

    cout << "\n=== FINAL MACHINE STATE ===" << endl;
    cout << "Total Cycles: " << cycleCount << endl;
    Debug::printRegisters(registers);
    Debug::printMemory(memory);
}
