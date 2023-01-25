; Espacio de 521MB alineado a 16 bits para guardar los registros de SIMD de la tarea 2

USE16
section .task2_simd PROGBITS

GLOBAL  Task2_SIMD_Registers

ALIGN 16

Task2_SIMD_Registers: TIMES 512 db 0
