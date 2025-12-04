# Simple MIPS Test - Basic arithmetic
# Assumes no hazards in this sequence
# CS3339 Fall 2025

        ADDI $t0, $zero, 42    # t0 = 42
        ADDI $t1, $zero, 8     # t1 = 8
        ADD  $t2, $zero, $zero # t2 = 0 (use later)
        NOP
        NOP
        ADD  $t2, $t0, $t1     # t2 = 42 + 8 = 50
