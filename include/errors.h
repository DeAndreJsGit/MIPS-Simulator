#ifndef ERRORS_H
#define ERRORS_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// Represents a single error with line number and message
struct Error {
    int line;
    std::string message;
    
    Error(int ln, const std::string& msg) : line(ln), message(msg) {}
};

// Error collection and reporting
class ErrorHandler {
private:
    std::vector<Error> errors;
    
public:
    void addError(int line, const std::string& message);
    bool hasErrors() const;
    size_t errorCount() const;
    void printErrors() const;
    bool writeErrorFile(const std::string& filename) const;
    void clear();
};

#endif // ERRORS_H
