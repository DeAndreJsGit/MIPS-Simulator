#ifndef PARSER_H
#define PARSER_H

#include "cpu.h"
#include "errors.h"
#include <string>
#include <istream>

// Declares the Parser class with parse() method and helper functions for reading assembly files

class Parser {
private:
    ErrorHandler& errorHandler;
    Program program;
    
    // Helper methods
    std::string trim(const std::string& s);
    std::string toUpper(const std::string& s);
    int parseRegister(const std::string& reg, int lineNum);
    Opcode parseOpcode(const std::string& mnemonic);
    std::vector<std::string> splitOperands(const std::string& operandStr);
    
public:
    Parser(ErrorHandler& eh);
    
    // Parse assembly from input stream
    Program parse(std::istream& input);
    
    // Check if parsing succeeded
    bool success() const;
};

#endif // PARSER_H
