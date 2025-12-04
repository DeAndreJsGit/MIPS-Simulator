#ifndef CPU_H
#define CPU_H

#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <unordered_map>

// Supported MIPS opcodes
enum class Opcode {
    ADD, ADDI, SUB, MUL,
    AND, OR, SLL, SRL,
    LW, SW, BEQ, J, NOP,
    UNKNOWN
};

// Parsed instruction
struct Instruction {
    Opcode op;
    int rs;         // Source register 1
    int rt;         // Source register 2 / destination for I-type
    int rd;         // Destination register for R-type
    int shamt;      // Shift amount
    int32_t imm;    // Immediate value
    size_t target;  // Jump/branch target (instruction index)
    std::string text;  // Original text for debug
    
    Instruction() : op(Opcode::NOP), rs(0), rt(0), rd(0), shamt(0), imm(0), target(0) {}
};

// Control signals generated during decode
struct ControlSignals {
    bool regDst;    // 1=rd (R-type), 0=rt (I-type)
    bool aluSrc;    // 1=immediate, 0=register
    bool memToReg;  // 1=memory data, 0=ALU result
    bool regWrite;  // Write to register file
    bool memRead;   // Read from memory
    bool memWrite;  // Write to memory
    bool branch;    // Branch instruction
    bool jump;      // Jump instruction
    
    ControlSignals() : regDst(false), aluSrc(false), memToReg(false),
                       regWrite(false), memRead(false), memWrite(false),
                       branch(false), jump(false) {}
};

// IF/ID pipeline register
struct IF_ID {
    bool valid;
    size_t pc;
    Instruction instr;
    
    IF_ID() : valid(false), pc(0) {}
};

// ID/EX pipeline register
struct ID_EX {
    bool valid;
    size_t pc;
    Instruction instr;
    ControlSignals ctrl;
    int32_t rsVal;      // Value read from rs
    int32_t rtVal;      // Value read from rt
    int32_t signExtImm; // Sign-extended immediate
    int destReg;        // Destination register number
    
    ID_EX() : valid(false), pc(0), rsVal(0), rtVal(0), signExtImm(0), destReg(0) {}
};

// EX/MEM pipeline register
struct EX_MEM {
    bool valid;
    size_t pc;
    Instruction instr;
    ControlSignals ctrl;
    int32_t aluResult;
    int32_t rtVal;       // For SW
    int destReg;
    bool branchTaken;
    size_t branchTarget;
    
    EX_MEM() : valid(false), pc(0), aluResult(0), rtVal(0), destReg(0),
               branchTaken(false), branchTarget(0) {}
};

// MEM/WB pipeline register
struct MEM_WB {
    bool valid;
    size_t pc;
    Instruction instr;
    ControlSignals ctrl;
    int32_t aluResult;
    int32_t memReadData;
    int destReg;
    
    MEM_WB() : valid(false), pc(0), aluResult(0), memReadData(0), destReg(0) {}
};

// Program loaded from assembly file
struct Program {
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, size_t> labels;
};

// The CPU class that runs the simulation
class CPU {
private:
    // Program
    std::vector<Instruction> instructions;
    
    // Architectural state
    size_t pc;
    std::array<int32_t, 32> registers;
    std::vector<int32_t> memory;
    
    // Pipeline registers
    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;
    
    // Statistics
    size_t cycleCount;
    bool debugMode;
    
    // Helper methods
    ControlSignals generateControl(const Instruction& instr);
    int32_t executeALU(const Instruction& instr, int32_t op1, int32_t op2);
    bool pipelineEmpty() const;
    
public:
    CPU(const Program& prog, bool debug = false);
    
    void run();
    void stepPipeline();
    
    // Accessors for debug output
    const std::array<int32_t, 32>& getRegisters() const { return registers; }
    const std::vector<int32_t>& getMemory() const { return memory; }
    size_t getPC() const { return pc; }
    size_t getCycleCount() const { return cycleCount; }
    
    const IF_ID& getIF_ID() const { return if_id; }
    const ID_EX& getID_EX() const { return id_ex; }
    const EX_MEM& getEX_MEM() const { return ex_mem; }
    const MEM_WB& getMEM_WB() const { return mem_wb; }
    
    bool isDebugMode() const { return debugMode; }
};

// Convert opcode to string
std::string opcodeToString(Opcode op);

#endif // CPU_H
