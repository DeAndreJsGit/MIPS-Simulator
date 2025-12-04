#ifndef STAGES_H
#define STAGES_H

#include "cpu.h"

// Pipeline stage functions
// These operate on the CPU state and update the pipeline registers

class PipelineStages {
public:
    // Execute Write Back stage
    static void wbStage(
        const MEM_WB& mem_wb,
        std::array<int32_t, 32>& registers
    );
    
    // Execute Memory stage
    static MEM_WB memStage(
        const EX_MEM& ex_mem,
        std::vector<int32_t>& memory
    );
    
    // Execute Execute stage
    // Returns: next EX_MEM, branchTaken flag, branchTarget
    static EX_MEM exStage(
        const ID_EX& id_ex,
        bool& branchTaken,
        size_t& branchTarget
    );
    
    // Execute Decode stage
    static ID_EX idStage(
        const IF_ID& if_id,
        const std::array<int32_t, 32>& registers
    );
    
    // Execute Fetch stage
    static IF_ID ifStage(
        size_t pc,
        const std::vector<Instruction>& instructions,
        bool flush
    );
    
    // Generate control signals for an instruction
    static ControlSignals generateControl(const Instruction& instr);
    
    // Execute ALU operation
    static int32_t executeALU(const Instruction& instr, int32_t op1, int32_t op2);
};

#endif // STAGES_H
