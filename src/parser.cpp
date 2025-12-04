#include "../include/parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

using namespace std;

Parser::Parser(ErrorHandler& eh) : errorHandler(eh) {}

string Parser::trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

string Parser::toUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

int Parser::parseRegister(const string& reg, int lineNum) {
    string r = trim(reg);
    
    // Remove $ prefix if present
    if (!r.empty() && r[0] == '$') {
        r = r.substr(1);
    }
    
    // Handle numeric registers ($0-$31)
    if (!r.empty() && isdigit(static_cast<unsigned char>(r[0]))) {
        int num = stoi(r);
        if (num < 0 || num > 31) {
            errorHandler.addError(lineNum, "Invalid register number: " + reg);
            return 0;
        }
        return num;
    }
    
    // Handle named registers
    string upper = toUpper(r);
    
    if (upper == "ZERO") return 0;
    if (upper == "AT") return 1;
    
    // $v0-$v1
    if (upper.length() == 2 && upper[0] == 'V') {
        int n = upper[1] - '0';
        if (n >= 0 && n <= 1) return 2 + n;
    }
    
    // $a0-$a3
    if (upper.length() == 2 && upper[0] == 'A') {
        int n = upper[1] - '0';
        if (n >= 0 && n <= 3) return 4 + n;
    }
    
    // $t0-$t9
    if (upper.length() == 2 && upper[0] == 'T') {
        int n = upper[1] - '0';
        if (n >= 0 && n <= 7) return 8 + n;
        if (n == 8 || n == 9) return 24 + (n - 8);
    }
    
    // $s0-$s7
    if (upper.length() == 2 && upper[0] == 'S') {
        int n = upper[1] - '0';
        if (n >= 0 && n <= 7) return 16 + n;
    }
    
    // $k0-$k1
    if (upper.length() == 2 && upper[0] == 'K') {
        int n = upper[1] - '0';
        if (n >= 0 && n <= 1) return 26 + n;
    }
    
    if (upper == "GP") return 28;
    if (upper == "SP") return 29;
    if (upper == "FP") return 30;
    if (upper == "RA") return 31;
    
    errorHandler.addError(lineNum, "Unknown register: " + reg);
    return 0;
}

Opcode Parser::parseOpcode(const string& mnemonic) {
    string upper = toUpper(mnemonic);
    
    if (upper == "ADD")  return Opcode::ADD;
    if (upper == "ADDI") return Opcode::ADDI;
    if (upper == "SUB")  return Opcode::SUB;
    if (upper == "MUL")  return Opcode::MUL;
    if (upper == "AND")  return Opcode::AND;
    if (upper == "OR")   return Opcode::OR;
    if (upper == "SLL")  return Opcode::SLL;
    if (upper == "SRL")  return Opcode::SRL;
    if (upper == "LW")   return Opcode::LW;
    if (upper == "SW")   return Opcode::SW;
    if (upper == "BEQ")  return Opcode::BEQ;
    if (upper == "J")    return Opcode::J;
    if (upper == "NOP")  return Opcode::NOP;
    
    return Opcode::UNKNOWN;
}

vector<string> Parser::splitOperands(const string& operandStr) {
    vector<string> operands;
    string current;
    int parenDepth = 0;
    
    for (char c : operandStr) {
        if (c == '(') parenDepth++;
        if (c == ')') parenDepth--;
        
        if (c == ',' && parenDepth == 0) {
            operands.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        operands.push_back(trim(current));
    }
    
    return operands;
}

Program Parser::parse(istream& input) {
    program.instructions.clear();
    program.labels.clear();
    
    // Store raw lines with their instruction text
    struct LineInfo {
        string text;
        int srcLine;
    };
    vector<LineInfo> instrLines;
    
    string line;
    int srcLineNum = 0;
    size_t instrIndex = 0;
    
    // First pass: collect all instruction lines and labels
    while (getline(input, line)) {
        srcLineNum++;
        
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) continue;
        
        // Check for label
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string label = trim(line.substr(0, colonPos));
            if (!label.empty()) {
                if (program.labels.count(label)) {
                    errorHandler.addError(srcLineNum, "Duplicate label: " + label);
                } else {
                    // Label points to next instruction index
                    program.labels[label] = instrIndex;
                }
            }
            line = trim(line.substr(colonPos + 1));
            if (line.empty()) continue;
        }
        
        // This is an instruction line
        instrLines.push_back({line, srcLineNum});
        instrIndex++;
    }
    
    // Second pass: parse instructions with label resolution
    for (size_t i = 0; i < instrLines.size(); i++) {
        const auto& info = instrLines[i];
        const string& instrLine = info.text;
        int lineNum = info.srcLine;
        
        istringstream iss(instrLine);
        string mnemonic;
        iss >> mnemonic;
        
        Opcode op = parseOpcode(mnemonic);
        if (op == Opcode::UNKNOWN) {
            errorHandler.addError(lineNum, "Unknown instruction: " + mnemonic);
            continue;
        }
        
        Instruction instr;
        instr.op = op;
        instr.text = instrLine;
        
        string rest;
        getline(iss, rest);
        rest = trim(rest);
        
        vector<string> operands = splitOperands(rest);
        
        switch (op) {
            case Opcode::NOP:
                break;
                
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MUL:
            case Opcode::AND:
            case Opcode::OR:
                if (operands.size() != 3) {
                    errorHandler.addError(lineNum, "Expected 3 operands for " + mnemonic);
                    continue;
                }
                instr.rd = parseRegister(operands[0], lineNum);
                instr.rs = parseRegister(operands[1], lineNum);
                instr.rt = parseRegister(operands[2], lineNum);
                break;
                
            case Opcode::ADDI:
                if (operands.size() != 3) {
                    errorHandler.addError(lineNum, "Expected 3 operands for ADDI");
                    continue;
                }
                instr.rt = parseRegister(operands[0], lineNum);
                instr.rs = parseRegister(operands[1], lineNum);
                try {
                    instr.imm = stoi(operands[2]);
                } catch (...) {
                    errorHandler.addError(lineNum, "Invalid immediate value: " + operands[2]);
                }
                break;
                
            case Opcode::SLL:
            case Opcode::SRL:
                if (operands.size() != 3) {
                    errorHandler.addError(lineNum, "Expected 3 operands for " + mnemonic);
                    continue;
                }
                instr.rd = parseRegister(operands[0], lineNum);
                instr.rt = parseRegister(operands[1], lineNum);
                try {
                    instr.shamt = stoi(operands[2]);
                    instr.imm = instr.shamt;
                } catch (...) {
                    errorHandler.addError(lineNum, "Invalid shift amount: " + operands[2]);
                }
                break;
                
            case Opcode::LW:
            case Opcode::SW:
                if (operands.size() != 2) {
                    errorHandler.addError(lineNum, "Expected 2 operands for " + mnemonic);
                    continue;
                }
                instr.rt = parseRegister(operands[0], lineNum);
                {
                    regex memRegex("(-?\\d+)\\((.+)\\)");
                    smatch match;
                    if (regex_match(operands[1], match, memRegex)) {
                        try {
                            instr.imm = stoi(match[1].str());
                        } catch (...) {
                            errorHandler.addError(lineNum, "Invalid offset: " + match[1].str());
                        }
                        instr.rs = parseRegister(match[2].str(), lineNum);
                    } else {
                        errorHandler.addError(lineNum, "Invalid memory operand format: " + operands[1]);
                    }
                }
                break;
                
            case Opcode::BEQ:
                if (operands.size() != 3) {
                    errorHandler.addError(lineNum, "Expected 3 operands for BEQ");
                    continue;
                }
                instr.rs = parseRegister(operands[0], lineNum);
                instr.rt = parseRegister(operands[1], lineNum);
                {
                    string label = trim(operands[2]);
                    if (program.labels.count(label)) {
                        instr.target = program.labels[label];
                    } else {
                        try {
                            int offset = stoi(label);
                            instr.target = i + 1 + offset;
                        } catch (...) {
                            errorHandler.addError(lineNum, "Undefined label: " + label);
                        }
                    }
                }
                break;
                
            case Opcode::J:
                if (operands.size() != 1) {
                    errorHandler.addError(lineNum, "Expected 1 operand for J");
                    continue;
                }
                {
                    string label = trim(operands[0]);
                    if (program.labels.count(label)) {
                        instr.target = program.labels[label];
                    } else {
                        try {
                            instr.target = stoul(label);
                        } catch (...) {
                            errorHandler.addError(lineNum, "Undefined label: " + label);
                        }
                    }
                }
                break;
                
            default:
                break;
        }
        
        program.instructions.push_back(instr);
    }
    
    return program;
}

bool Parser::success() const {
    return !errorHandler.hasErrors();
}
