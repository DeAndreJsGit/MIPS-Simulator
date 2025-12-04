// MIPS Pipeline Simulator - CS3339 
// A 5-stage pipelined MIPS processor simulator that executes
// assembly instructions and displays register/memory state.

#include <iostream>
#include <fstream>
#include <string>
#include "../include/parser.h"
#include "../include/cpu.h"
#include "../include/errors.h"
#include "../include/debug.h"

using namespace std;

void printUsage(const char* progName) {
    cerr << "MIPS Pipeline Simulator - CS3339 Fall 2025" << endl << endl;
    cerr << "Usage: " << progName << " <input.asm> [options]" << endl << endl;
    cerr << "Options:" << endl;
    cerr << "  --debug, -d    Show pipeline state after each cycle" << endl;
    cerr << "  --help, -h     Show this help message" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    string filename;
    bool debugMode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--debug" || arg == "-d") {
            debugMode = true;
        } else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (!arg.empty() && arg[0] == '-') {
            cerr << "Unknown option: " << arg << endl;
            printUsage(argv[0]);
            return 1;
        } else {
            filename = arg;
        }
    }
    
    if (filename.empty()) {
        cerr << "Error: No input file specified" << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Open input file
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return 1;
    }
    
    cout << "=== MIPS PIPELINE SIMULATOR ===" << endl;
    cout << "CS3339 Fall 2025" << endl;
    cout << "Input file: " << filename << endl;
    if (debugMode) {
        cout << "Debug mode: ENABLED" << endl;
    }
    
    // Parse the assembly file
    ErrorHandler errorHandler;
    Parser parser(errorHandler);
    
    Program program = parser.parse(inputFile);
    inputFile.close();
    
    // Check for parsing errors
    if (errorHandler.hasErrors()) {
        errorHandler.printErrors();
        
        // Generate error file name from input file
        string errorFile = filename;
        size_t dotPos = errorFile.rfind('.');
        if (dotPos != string::npos) {
            errorFile = errorFile.substr(0, dotPos);
        }
        errorFile += ".err";
        
        errorHandler.writeErrorFile(errorFile);
        return 1;
    }
    
    // Check if we have any instructions
    if (program.instructions.empty()) {
        cerr << "Error: No instructions found in input file" << endl;
        return 1;
    }
    
    cout << "Instructions loaded: " << program.instructions.size() << endl;
    
    // Create and run the CPU simulator
    try {
        CPU cpu(program, debugMode);
        cpu.run();
        
        cout << endl << "=== SIMULATION COMPLETE ===" << endl;
    } catch (const exception& e) {
        cerr << endl << "Runtime Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
