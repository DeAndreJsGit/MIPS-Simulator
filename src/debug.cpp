#include "../include/debug.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>

using namespace std;

string Debug::regName(int reg) {
    static const char* names[] = {
        "$zero", "$at", "$v0", "$v1",
        "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9", "$k0", "$k1",
        "$gp", "$sp", "$fp", "$ra"
    };
    if (reg >= 0 && reg < 32) return names[reg];
    return "$??";
}

void Debug::printRegisters(const std::array<int32_t, 32>& registers) {
    std::cout << "\n--- Register File ---\n";
    bool found = false;
    
    for (int i = 0; i < 32; i++) {
        if (registers[i] != 0) {
            std::cout << std::setw(6) << regName(i) << " ($" << std::setw(2) << i << "): "
                      << std::setw(11) << registers[i];
            
            // Print hex value
            std::cout << "  (0x" << std::hex << std::setfill('0') 
                      << std::setw(8) << static_cast<uint32_t>(registers[i])
                      << std::dec << std::setfill(' ') << ")\n";
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "  (all registers are zero)\n";
    }
}

void Debug::printMemory(const std::vector<int32_t>& memory) {
    std::cout << "\n--- Memory (non-zero) ---\n";
    bool found = false;
    
    for (size_t i = 0; i < memory.size(); i++) {
        if (memory[i] != 0) {
            std::cout << "  [" << std::setw(4) << (i * 4) << "]: "
                      << std::setw(11) << memory[i]
                      << "  (0x" << std::hex << std::setfill('0')
                      << std::setw(8) << static_cast<uint32_t>(memory[i])
                      << std::dec << std::setfill(' ') << ")\n";
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "  (all memory locations are zero)\n";
    }
}

void Debug::printControlSignals(const ControlSignals& ctrl) {
    std::cout << "    RegDst=" << ctrl.regDst
              << " ALUSrc=" << ctrl.aluSrc
              << " MemToReg=" << ctrl.memToReg
              << " RegWrite=" << ctrl.regWrite
              << " MemRead=" << ctrl.memRead
              << " MemWrite=" << ctrl.memWrite
              << " Branch=" << ctrl.branch
              << " Jump=" << ctrl.jump << "\n";
}

void Debug::printIF_ID(const IF_ID& reg) {
    std::cout << "  IF/ID: ";
    if (reg.valid) {
        std::cout << "[" << reg.instr.text << "]\n";
        std::cout << "    PC=" << reg.pc << "\n";
    } else {
        std::cout << "[empty]\n";
    }
}

void Debug::printID_EX(const ID_EX& reg) {
    std::cout << "  ID/EX: ";
    if (reg.valid) {
        std::cout << "[" << reg.instr.text << "]\n";
        std::cout << "    PC=" << reg.pc << "\n";
        printControlSignals(reg.ctrl);
        std::cout << "    " << regName(reg.instr.rs) << "=" << reg.rsVal
                  << ", " << regName(reg.instr.rt) << "=" << reg.rtVal
                  << ", SignExtImm=" << reg.signExtImm
                  << ", DestReg=" << regName(reg.destReg) << "\n";
    } else {
        std::cout << "[empty]\n";
    }
}

void Debug::printEX_MEM(const EX_MEM& reg) {
    std::cout << "  EX/MEM: ";
    if (reg.valid) {
        std::cout << "[" << reg.instr.text << "]\n";
        std::cout << "    PC=" << reg.pc << "\n";
        printControlSignals(reg.ctrl);
        std::cout << "    ALUResult=" << reg.aluResult
                  << ", DestReg=" << regName(reg.destReg);
        if (reg.branchTaken) {
            std::cout << ", BRANCH TAKEN to " << reg.branchTarget;
        }
        std::cout << "\n";
    } else {
        std::cout << "[empty]\n";
    }
}

void Debug::printMEM_WB(const MEM_WB& reg) {
    std::cout << "  MEM/WB: ";
    if (reg.valid) {
        std::cout << "[" << reg.instr.text << "]\n";
        std::cout << "    PC=" << reg.pc << "\n";
        printControlSignals(reg.ctrl);
        std::cout << "    ALUResult=" << reg.aluResult;
        if (reg.ctrl.memToReg) {
            std::cout << ", MemData=" << reg.memReadData;
        }
        std::cout << ", DestReg=" << regName(reg.destReg) << "\n";
    } else {
        std::cout << "[empty]\n";
    }
}

void Debug::printPipelineState(const CPU& cpu) {
    std::cout << "\n========== CYCLE " << cpu.getCycleCount() << " ==========\n";
    std::cout << "PC = " << cpu.getPC() << "\n\n";
    
    std::cout << "--- Pipeline Registers ---\n";
    printIF_ID(cpu.getIF_ID());
    printID_EX(cpu.getID_EX());
    printEX_MEM(cpu.getEX_MEM());
    printMEM_WB(cpu.getMEM_WB());
    
    printRegisters(cpu.getRegisters());
    std::cout << "================================\n";
}

void Debug::printBinaryRepresentation(const std::vector<Instruction>& instructions) {
    std::cout << "\n--- Binary Representation ---" << std::endl;
    std::cout << std::setw(4) << "Addr" << "  " << std::setw(32) << "Binary" 
              << "  " << "Assembly" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (size_t i = 0; i < instructions.size(); i++) {
        const Instruction& instr = instructions[i];
        uint32_t binary = 0;
        
        switch (instr.op) {
            case Opcode::ADD:
                binary = (0 << 26) | (instr.rs << 21) | (instr.rt << 16) 
                       | (instr.rd << 11) | (0 << 6) | 0x20;
                break;
            case Opcode::SUB:
                binary = (0 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (0 << 6) | 0x22;
                break;
            case Opcode::MUL:
                binary = (0x1C << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (0 << 6) | 0x02;
                break;
            case Opcode::AND:
                binary = (0 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (0 << 6) | 0x24;
                break;
            case Opcode::OR:
                binary = (0 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (0 << 6) | 0x25;
                break;
            case Opcode::SLL:
                binary = (0 << 26) | (0 << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (instr.shamt << 6) | 0x00;
                break;
            case Opcode::SRL:
                binary = (0 << 26) | (0 << 21) | (instr.rt << 16)
                       | (instr.rd << 11) | (instr.shamt << 6) | 0x02;
                break;
            case Opcode::ADDI:
                binary = (0x08 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.imm & 0xFFFF);
                break;
            case Opcode::LW:
                binary = (0x23 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.imm & 0xFFFF);
                break;
            case Opcode::SW:
                binary = (0x2B << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (instr.imm & 0xFFFF);
                break;
            case Opcode::BEQ:
                binary = (0x04 << 26) | (instr.rs << 21) | (instr.rt << 16)
                       | (static_cast<int32_t>(instr.target - i - 1) & 0xFFFF);
                break;
            case Opcode::J:
                binary = (0x02 << 26) | (instr.target & 0x03FFFFFF);
                break;
            case Opcode::NOP:
                binary = 0;
                break;
            default:
                binary = 0xFFFFFFFF;
        }
        
        std::cout << std::setw(4) << (i * 4) << "  "
                  << std::bitset<32>(binary) << "  "
                  << instr.text << "\n";
    }
    std::cout << std::endl;
}
