USE16
section .resetVector

EXTERN start16
GLOBAL reset

; punto de entrada
reset:
    cli
    cld
    jmp start16
    halted:
        hlt
        jmp halted
end:
