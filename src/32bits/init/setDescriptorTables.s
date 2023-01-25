; llama a las funciones del Descriptor Handler y hace el load de los registros de Segmentos

USE32
section .set_dt

EXTERN _gdtr
EXTERN _idtr
EXTERN SetGlobalDescriptionTables
EXTERN SetInterruptDescriptionTables
EXTERN __CS_SEL_32_INDEX
EXTERN kernel32_init
EXTERN ReprogramarPICs

GLOBAL __setDescriptorTables__


__setDescriptorTables__:
    push    ebp
    mov     ebp, esp

    call    SetGlobalDescriptionTables
    lgdt    [_gdtr]

    call    SetInterruptDescriptionTables
    lidt    [_idtr]

    mov esp, ebp
    pop ebp
    ret
