# **MIPS Pipeline Simulator**

### **CS3339 – Fall 2025 Course Project**

A 5-stage pipelined MIPS processor simulator written in C++.
The simulator created loads a MIPS assembly program, decodes each instruction,
executes the instruction through a pipeline, and displays the final register and memory state.

---

# **Features**

* **5-Stage Pipeline**
  Faithful implementation of **IF, ID, EX, MEM, WB**

* **Pipeline Registers**
  IF/ID, ID/EX, EX/MEM, MEM/WB

* **Control Signal Generation**
  Produced during decode stage

* **Debug Mode**
  Cycle-by-cycle pipeline visualization

* **Binary Instruction Output**
  Shows each instruction’s 32-bit encoding

* **Error Handling**
  Invalid instructions or malformed assembly generate `.err` files

---

# **Supported Instructions**

| Opcode | Description         |
| ------ | ------------------- |
| ADD    | Signed integer add  |
| ADDI   | Add immediate       |
| SUB    | Signed subtract     |
| MUL    | Integer multiply    |
| AND    | Bitwise AND         |
| OR     | Bitwise OR          |
| SLL    | Shift left logical  |
| SRL    | Shift right logical |
| LW     | Load word           |
| SW     | Store word          |
| BEQ    | Branch if equal     |
| J      | Jump                |
| NOP    | No operation        |

---

# **Project Structure**

```
MIPS-Simulator/
│
├── src/
│   ├── main.cpp       # Entry point
│   ├── parser.cpp     # Assembly parsing
│   ├── cpu.cpp        # CPU + simulation loop
│   ├── stages.cpp     # IF/ID/EX/MEM/WB logic
│   ├── debug.cpp      # Debug printing
│   └── errors.cpp     # Error reporting
│
├── include/
│   ├── parser.h
│   ├── cpu.h
│   ├── stages.h
│   ├── debug.h
│   └── errors.h
│
├── tests/             # Test .asm files
├── Makefile
└── README.md
```

---
# **Building the Project**

## **Requirements**

* C++17-compatible compiler (`g++` recommended)
* Make utility

---

## **Building on Windows (MinGW)**

If you are using **MinGW**, the build tool is:

```
mingw32-make
```

### Build:

```
mingw32-make
```

### Debug build:

```
mingw32-make debug
```

### Clean:

```
mingw32-make clean
```

---

## **Building on Linux / macOS**

(Using standard GNU Make)

### Build:

```
make
```

### Debug build:

```
make debug
```

### Clean:

```
make clean
```

---

# **Usage**

### **Basic Execution**

```
./mips_sim <input.asm>
```

### **With Debug Mode**

```
./mips_sim <input.asm> --debug
```

or

```
./mips_sim <input.asm> -d
```

### **Help**

```
./mips_sim --help
```

---

# **Input Format**

* Comments start with `#`
* Labels end with `:`
* Registers may be numbered or named
  (`$0`, `$8`, `$t0`, `$s0`, `$zero`, etc.)
* Memory operands use:

  ```
  offset(base)
  ```

## **Debug Mode**

Additionally displays **each cycle**:

* Pipeline registers
* Control signals
* Program counter
* Register file snapshot

---

### **Example Program**

```assembly
# Simple arithmetic test
ADDI $t0, $zero, 10      # t0 = 10
ADDI $t1, $zero, 5       # t1 = 5
ADD  $t2, $t0, $t1       # t2 = 15
SUB  $t3, $t0, $t1       # t3 = 5
SW   $t2, 0($zero)       # mem[0] = 15
LW   $t4, 0($zero)       # t4 = 15
```

---

# **Output Description**

## **Normal Mode**

Shows:

* Instruction count
* Binary encoding table
* Final register file
* Final memory state
* Total cycle count

---

# **Design Principles**

* **Parser**
  Cleans input assembly, extracts labels, constructs `Instruction` objects

* **CPU**
  Manages registers, memory, PC, and pipeline registers
  Controls simulation loop + branching logic

* **Pipeline Stages**
  Each stage implemented as its own function for clarity

* **Debug System**
  All printing and formatting isolated in one module

* **Error System**
  Detects invalid syntax and produces `.err` diagnostic files

---

# **Error Handling**

The simulator detects:

* Unknown or misspelled instructions
* Invalid register names
* Undefined labels
* Bad immediate values
* Incorrect operand formats

If errors occur:

1. Error message prints to console
2. A `.err` file is generated with details
3. Simulation does not start

---

# **Notes**

* Input programs should be **free of hazards** (as permitted by the spec)
* Memory is **word-addressable** (4 bytes per word)
* Memory size: **1024 words** (4 KB)
* All registers initialize to 0
* `$zero` is always forced to 0

---

# **Author**

**DeAndre Johnson - Texas State University**
