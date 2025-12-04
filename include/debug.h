#ifndef DEBUG_H
#define DEBUG_H

#include "cpu.h"
#include <array>
#include <vector>
using namespace std;

// Declares Debug class with static print functions for registers, memory, pipeline state, and binary representation

class Debug {
public:
    // Print register file contents
    static void printRegisters(const std::array<int32_t, 32>& registers);
    
    // Print non-zero memory locations
    static void printMemory(const std::vector<int32_t>& memory);
    
    // Print control signals
    static void printControlSignals(const ControlSignals& ctrl);
    
    // Print pipeline register contents
    static void printIF_ID(const IF_ID& reg);
    static void printID_EX(const ID_EX& reg);
    static void printEX_MEM(const EX_MEM& reg);
    static void printMEM_WB(const MEM_WB& reg);
    
    // Print full pipeline state (called each cycle in debug mode)
    static void printPipelineState(const CPU& cpu);
    
    // Print binary representation of instructions
    static void printBinaryRepresentation(const std::vector<Instruction>& instructions);
    
    // Get register name from number
    static string regName(int reg);
};

#endif // DEBUG_H
