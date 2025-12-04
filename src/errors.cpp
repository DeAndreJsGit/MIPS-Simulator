#include "../include/errors.h"
#include <iostream>
#include <fstream>

using namespace std;

void ErrorHandler::addError(int line, const string& message) {
    errors.emplace_back(line, message);
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

size_t ErrorHandler::errorCount() const {
    return errors.size();
}

void ErrorHandler::printErrors() const {
    cerr << "\n=== ASSEMBLY ERRORS ===" << endl;   // safe use of endl

    for (const auto& err : errors) {
        cerr << "Line " << err.line << ": " << err.message << "\n";
    }

    cerr << "Total errors: " << errors.size() << endl;   // safe use of endl
}

bool ErrorHandler::writeErrorFile(const string& filename) const {
    ofstream out(filename);
    if (!out) {
        cerr << "Could not create error file: " << filename << endl;
        return false;
    }
    
    out << "MIPS Assembly Errors" << endl;
    out << "====================" << endl;
    out << endl;
    
    for (const auto& err : errors) {
        out << "Line " << err.line << ": " << err.message << endl;
    }
    
    out << endl;
    out << "Total errors: " << errors.size() << endl;

    out.close();
    
    cout << "Errors written to: " << filename << endl;
    return true;
}

void ErrorHandler::clear() {
    errors.clear();
}
