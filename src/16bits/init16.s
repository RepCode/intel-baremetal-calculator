USE16
section .init16

GLOBAL  A20_Enable_No_Stack_return

EXTERN  __STACK_START_16
EXTERN  __STACK_END_16
EXTERN  CS_SEL_32
EXTERN  _init_gdtr
EXTERN  __start32_launcher__
EXTERN  A20_Enable_No_Stack
EXTERN  VGA_Init

%define X86_CR0_NW   0x20000000
%define X86_CR0_PE   0x00000001
%define X86_CR0_CD   0x40000000

GLOBAL  start16

start16:
    %ifdef DEBUG 
        xchg bx, bx
    %endif
    test    eax, 0x0; chequear que el micro no este en fallo
    jne     fault_end

    jmp A20_Enable_No_Stack
A20_Enable_No_Stack_return:

    xor     eax, eax
    mov     cr3, eax    ; invalida TLB

    ; Init stack de 16 bits
    mov     ax, cs
    mov     ds, ax; no copia la parte oculta del registro
    mov     ax, __STACK_START_16
    mov     ss, ax
    mov     sp, __STACK_END_16

    ; Deshabilitar Cache
    mov     eax, cr0
    or      eax, (X86_CR0_NW | X86_CR0_CD)
    mov     cr0, eax
    wbinvd  ; Write back and invalidate cache
    
    ; seteo la gdt basica de booteo
    lgdt    [_init_gdtr]

    ; llamo a la rutina de inicializacion de VGA
    push    bp
    mov     bp, sp
    call    VGA_Init
    leave

    %ifdef DEBUG 
        xchg bx, bx
    %endif
    ;Establecer el micro en MP
    smsw    ax
    or      ax, X86_CR0_PE
    lmsw    ax
    ;  Estoy en modo protegido
    jmp     dword   CS_SEL_32:__start32_launcher__

    fault_end:
        hlt
