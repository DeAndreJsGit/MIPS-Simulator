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

ControlSignals CPU::generateControl(const Instruction& instr) {
    return PipelineStages::generateControl(instr);
}

int32_t CPU::executeALU(const Instruction& instr, int32_t op1, int32_t op2) {
    return PipelineStages::executeALU(instr, op1, op2);
}

bool CPU::pipelineEmpty() const {
    return !if_id.valid && !id_ex.valid && !ex_mem.valid && !mem_wb.valid;
}

void CPU::stepPipeline() {
    // Next cycle values
    IF_ID next_if_id;
    ID_EX next_id_ex;
    EX_MEM next_ex_mem;
    MEM_WB next_mem_wb;

    // === WB Stage ===
    PipelineStages::wbStage(mem_wb, registers);

    // === MEM Stage ===
    next_mem_wb = PipelineStages::memStage(ex_mem, memory);

    // === EX Stage ===
    bool branchTaken = false;
    size_t branchTarget = 0;
    next_ex_mem = PipelineStages::exStage(id_ex, branchTaken, branchTarget);

    // === ID Stage ===
    next_id_ex = PipelineStages::idStage(if_id, registers);

    // === IF Stage ===
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
