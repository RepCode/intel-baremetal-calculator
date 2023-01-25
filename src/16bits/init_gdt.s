USE16
SECTION .init_gdt progbits

EXTERN  __INIT_GDT_VMA

GLOBAL  DS_SEL
GLOBAL  CS_SEL_32
GLOBAL  _init_gdtr

; GDT basica utilizada solo durante la inicializacion

GDT:
NULL_SEL    equ $-GDT
    dq  0x0
CS_SEL_32      equ $-GDT
    dw 0xFFFF       ;Limit  15-0
    dw 0x0000       ;Base  15-0
    db 0x00         ;Base  23-16
    db 10011001b    ;Atributos:
                    ;P=1
                    ;DPL=00
                    ;S=1
                    ;D/C=1
                    ;ED/C=0
                    ;R/W=0
                    ;A=1                    
    db 11001111b    ;Limit 19-16 + atributos:
                    ;G=1
                    ;D/B=1
                    ;L=0
                    ;AVL=0
    db 0x00         ;Base 31-24
DS_SEL      equ $-GDT
    dw 0xFFFF       ;Limit  15-0
    dw 0x0000       ;Base  15-0
    db 0x00         ;Base  23-16
    db 10010010b    ;Atributos:
                    ;P=1
                    ;DPL=00
                    ;S=1
                    ;D/C=0
                    ;ED/C=0
                    ;R/W=1
                    ;A=0
    db 11001111b    ;Atributos + Limit 19-16:
                    ;G=1
                    ;D/B=1
                    ;L=0
                    ;AVL=0
    db 0x00         ;Base 31-24    
GDT_LENGTH  equ $-GDT

_init_gdtr:
    dw  GDT_LENGTH-1
    dd  (__INIT_GDT_VMA-0xFFF00000)
