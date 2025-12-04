# MIPS Test Program - Uses all supported instructions
# NO DATA HAZARDS - properly scheduled
# CS3339 Fall 2025

# Initialize values with enough NOPs to avoid hazards
        ADDI $t0, $zero, 10    # t0 = 10
        NOP
        NOP
        NOP
        ADDI $t1, $zero, 5     # t1 = 5
        NOP
        NOP
        NOP
        
# Arithmetic (t0 and t1 are now stable)
        ADD  $t2, $t0, $t1     # t2 = 10 + 5 = 15
        NOP
        NOP
        NOP
        SUB  $t3, $t0, $t1     # t3 = 10 - 5 = 5
        NOP
        NOP
        NOP
        MUL  $t4, $t0, $t1     # t4 = 10 * 5 = 50
        NOP
        NOP
        NOP

# Logical
        ADDI $t5, $zero, 15    # t5 = 0x0F
        NOP
        NOP
        NOP
        ADDI $t6, $zero, 240   # t6 = 0xF0
        NOP
        NOP
        NOP
        AND  $t7, $t5, $t6     # t7 = 0x0F & 0xF0 = 0
        NOP
        NOP
        NOP
        OR   $t8, $t5, $t6     # t8 = 0x0F | 0xF0 = 0xFF = 255
        NOP
        NOP
        NOP

# Shift
        ADDI $s0, $zero, 4     # s0 = 4
        NOP
        NOP
        NOP
        SLL  $s1, $s0, 2       # s1 = 4 << 2 = 16
        NOP
        NOP
        NOP
        SRL  $s2, $s0, 1       # s2 = 4 >> 1 = 2
        NOP
        NOP
        NOP

# Memory - use t2 which is now 15
        SW   $t2, 0($zero)     # mem[0] = 15
        NOP
        NOP
        NOP
        SW   $t4, 4($zero)     # mem[4] = 50
        NOP
        NOP
        NOP
        LW   $s3, 0($zero)     # s3 = mem[0] = 15
        NOP
        NOP
        NOP
        LW   $s4, 4($zero)     # s4 = mem[4] = 50
        NOP
        NOP
        NOP

# Branch test - s5 should be 100, not 999
        ADDI $s5, $zero, 100   # s5 = 100
        NOP
        NOP
        NOP
        BEQ  $zero, $zero, skip  # always branch
        NOP                      # branch delay slot (gets flushed)
        ADDI $s5, $zero, 999   # SKIPPED
skip:
        ADDI $s6, $zero, 200   # s6 = 200
