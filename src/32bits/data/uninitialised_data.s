; Defino variables globales no inicializadas necesarias para el Kernel (no van en ROM)

USE32
section .uninitialised_data progbits

GLOBAL  Keyboard_Buffer
GLOBAL  Keyboard_Write_Index
GLOBAL  Interrupt_Counter
GLOBAL  Keyboard_Is_Release
GLOBAL  DigitsTableIndex
GLOBAL  NewStack
GLOBAL  CurrentSimdRegisters

ALIGN 32

Keyboard_Buffer: TIMES  9 db  0
Keyboard_Write_Index:   db  0
Keyboard_Is_Release:    db  0
Interrupt_Counter:      db  0
DigitsTableIndex:       dd  0
NewStack:   TIMES 4     dw  0
CurrentSimdRegisters:   dd  0