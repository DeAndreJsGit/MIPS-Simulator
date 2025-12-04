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

// Step 1: First thing the program does is enter main
int main(int argc, char* argv[]) {
    // Step 2: Check that Arguments/File were provided by user
    // If not, just print usage info and exit
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    string filename;
    bool debugMode = false;
    
    // Step 3: Parse/Check command line arguments
    // While parsing here, we can set flags before starting the simulation
    // If user types bad args, we can catch it and go Unknown option and lists options and exit
    // We Do Not want to run the simulation if args are bad
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
    
    // Step 4: Ensure the user actually provided an input file name
    if (filename.empty()) {
        // if no filename provided, print error and exit
        cerr << "Error: No input file specified" << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Step 5: Try to open the input file
    ifstream inputFile(filename);
    if (!inputFile) {
        // Could not open file, print error and exit
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return 1;
    }
    
    // Runs when file is successfully opened
    cout << "=== MIPS PIPELINE SIMULATOR ===" << endl;
    cout << "CS3339 Fall 2025" << endl;
    cout << "Input file: " << filename << endl;
    if (debugMode) {
        cout << "Debug mode: ENABLED" << endl;
    }
    
    // After successfully opening the file..
    // Step 6: ErrorHandler is created to collect any parsing errors from errors.cpp
    ErrorHandler errorHandler;
    Parser parser(errorHandler);
    
    // Step 7: Parse the assembly file with parser.cpp
    //   - Reads each line of assembly
    //   - Identifies labels 
    //   - Parses instructions (opcode, registers, immediates)
    //   - Creates Instruction objects with all fields filled in
    //   - Reports errors to ErrorHandler if syntax is bad
    Program program = parser.parse(inputFile);
    inputFile.close();
    
    // Step 8: Check for parsing errors
    // errors.cpp checks if any errors were collected
    if (errorHandler.hasErrors()) {
        errorHandler.printErrors();
        
        // If the assembly file is invalid, it stops here
        // Generate error file name from input file
        // For example, input.asm -> input.err
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
    
    // If Parsing Succeeds..
    // Step 9: Create the CPU simulator
    // The CPU constructor in cpu.cpp initializes:
    // - Pipeline registers (IF_ID, ID_EX, EX_MEM, MEM_WB)
    // - 32 registers, all set to 0
    // - 1024 words of memory
    // Step 10: Run the simulation
    // cpu.run() executes the pipeline cycle by cycle until complete
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
