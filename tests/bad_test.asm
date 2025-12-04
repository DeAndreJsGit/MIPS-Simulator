# Bad MIPS file with errors
        ADDI $t0, $zero, 10
        BADOP $t1, $t2         # Unknown instruction
        ADD $t3                # Wrong number of operands
        LW $t4, badformat      # Bad memory format
        BEQ $t0, $t1, undefined_label  # Undefined label
