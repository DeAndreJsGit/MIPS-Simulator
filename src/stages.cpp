#include "../include/stages.h"
#include <stdexcept>
#include <string>

using namespace std;

ControlSignals PipelineStages::generateControl(const Instruction& instr) {
    ControlSignals ctrl;
    
    switch (instr.op) {
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::AND:
        case Opcode::OR:
            ctrl.regDst = true;    // Write to rd
            ctrl.aluSrc = false;   // Use register
            ctrl.regWrite = true;  // Write result
            break;
            
        case Opcode::SLL:
        case Opcode::SRL:
            ctrl.regDst = true;    // Write to rd
            ctrl.aluSrc = false;   // rt value goes through
            ctrl.regWrite = true;
            break;
            
        case Opcode::ADDI:
            ctrl.regDst = false;   // Write to rt
            ctrl.aluSrc = true;    // Use immediate
            ctrl.regWrite = true;
            break;
            
        case Opcode::LW:
            ctrl.regDst = false;   // Write to rt
            ctrl.aluSrc = true;    // Use immediate for offset
            ctrl.memToReg = true;  // Data from memory
            ctrl.memRead = true;
            ctrl.regWrite = true;
            break;
            
        case Opcode::SW:
            ctrl.aluSrc = true;    // Use immediate for offset
            ctrl.memWrite = true;
            break;
            
        case Opcode::BEQ:
            ctrl.branch = true;
            break;
            
        case Opcode::J:
            ctrl.jump = true;
            break;
            
        case Opcode::NOP:
        default:
            // All signals false (default)
            break;
    }
    
    return ctrl;
}

int32_t PipelineStages::executeALU(const Instruction& instr, int32_t op1, int32_t op2) {
    switch (instr.op) {
        case Opcode::ADD:
        case Opcode::ADDI:
            return op1 + op2;
            
        case Opcode::SUB:
            return op1 - op2;
            
        case Opcode::MUL:
            return op1 * op2;
            
        case Opcode::AND:
            return op1 & op2;
            
        case Opcode::OR:
            return op1 | op2;
            
        case Opcode::SLL:
            return op2 << (instr.imm & 0x1F);
            
        case Opcode::SRL:
            return static_cast<int32_t>(static_cast<uint32_t>(op2) >> (instr.imm & 0x1F));
            
        case Opcode::LW:
        case Opcode::SW:
            return op1 + op2;  // Address calculation
            
        case Opcode::BEQ:
            return op1 - op2;  // Comparison
            
        default:
            return 0;
    }
}

void PipelineStages::wbStage(
    const MEM_WB& mem_wb,
    array<int32_t, 32>& registers
) {
    if (!mem_wb.valid) return;
    
    if (mem_wb.ctrl.regWrite && mem_wb.destReg != 0) {
        int32_t value = mem_wb.ctrl.memToReg 
                      ? mem_wb.memReadData 
                      : mem_wb.aluResult;
        registers[mem_wb.destReg] = value;
    }
}

MEM_WB PipelineStages::memStage(
    const EX_MEM& ex_mem,
    vector<int32_t>& memory
) {
    MEM_WB next;
    
    if (!ex_mem.valid) return next;
    
    next.valid = true;
    next.pc = ex_mem.pc;
    next.instr = ex_mem.instr;
    next.ctrl = ex_mem.ctrl;
    next.aluResult = ex_mem.aluResult;
    next.destReg = ex_mem.destReg;
    
    if (ex_mem.ctrl.memRead) {
        // Convert byte address to word index
        size_t addr = static_cast<size_t>(ex_mem.aluResult) / 4;
        if (addr >= memory.size()) {
            throw runtime_error("Memory read out of bounds at address " + 
                                to_string(ex_mem.aluResult));
        }
        next.memReadData = memory[addr];
    } else if (ex_mem.ctrl.memWrite) {
        size_t addr = static_cast<size_t>(ex_mem.aluResult) / 4;
        if (addr >= memory.size()) {
            throw runtime_error("Memory write out of bounds at address " + 
                                to_string(ex_mem.aluResult));
        }
        memory[addr] = ex_mem.rtVal;
    }
    
    return next;
}

EX_MEM PipelineStages::exStage(
    const ID_EX& id_ex,
    bool& branchTaken,
    size_t& branchTarget
) {
    EX_MEM next;
    branchTaken = false;
    branchTarget = 0;
    
    if (!id_ex.valid) return next;
    
    next.valid = true;
    next.pc = id_ex.pc;
    next.instr = id_ex.instr;
    next.ctrl = id_ex.ctrl;
    next.destReg = id_ex.destReg;
    next.rtVal = id_ex.rtVal;
    
    // Select ALU operand 2
    int32_t aluOp2 = id_ex.ctrl.aluSrc ? id_ex.signExtImm : id_ex.rtVal;
    
    // Execute ALU
    next.aluResult = executeALU(id_ex.instr, id_ex.rsVal, aluOp2);
    
    // Check for branch/jump
    if (id_ex.instr.op == Opcode::BEQ && id_ex.rsVal == id_ex.rtVal) {
        branchTaken = true;
        branchTarget = id_ex.instr.target;
    }
    
    if (id_ex.instr.op == Opcode::J) {
        branchTaken = true;
        branchTarget = id_ex.instr.target;
    }
    
    next.branchTaken = branchTaken;
    next.branchTarget = branchTarget;
    
    return next;
}

ID_EX PipelineStages::idStage(
    const IF_ID& if_id,
    const array<int32_t, 32>& registers
) {
    ID_EX next;
    
    if (!if_id.valid) return next;
    
    const Instruction& instr = if_id.instr;
    
    next.valid = true;
    next.pc = if_id.pc;
    next.instr = instr;
    next.ctrl = generateControl(instr);
    
    // Read register values
    next.rsVal = registers[instr.rs];
    next.rtVal = registers[instr.rt];
    
    // Sign extend immediate
    int16_t imm16 = static_cast<int16_t>(instr.imm & 0xFFFF);
    next.signExtImm = static_cast<int32_t>(imm16);
    
    // Determine destination register
    if (next.ctrl.regDst) {
        next.destReg = instr.rd;  // R-type
    } else if (next.ctrl.regWrite) {
        next.destReg = instr.rt;  // I-type
    } else {
        next.destReg = 0;
    }
    
    return next;
}

IF_ID PipelineStages::ifStage(
    size_t pc,
    const vector<Instruction>& instructions,
    bool flush
) {
    IF_ID next;
    
    if (flush || pc >= instructions.size()) {
        return next;  // Invalid/empty
    }
    
    next.valid = true;
    next.pc = pc;
    next.instr = instructions[pc];
    
    return next;
}
