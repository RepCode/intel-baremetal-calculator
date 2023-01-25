; Espacio de 521MB alineado a 16 bits para guardar los registros de SIMD de la tarea 1

USE16

section .task1_simd PROGBITS

GLOBAL  Task1_SIMD_Registers

ALIGN 16

Task1_SIMD_Registers: TIMES 512 db 0