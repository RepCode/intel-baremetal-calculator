; ultimos pasos de inicializacion previo a inicializar el scheduler

USE32
section .kernel32

EXTERN  __reprogramPICs_
EXTERN  __PIT_Set_Counter0__
EXTERN  SetScheduler
EXTERN  __keyboard_Setup__

GLOBAL  __kernel32_init__

__kernel32_init__:

    .main:
        push ebp
        mov ebp, esp

        call    __reprogramPICs_
        call    __keyboard_Setup__

        push ebp
        mov ebp, esp
        mov cx, 0d10000 ; para interrumpir cada 100ms
        call __PIT_Set_Counter0__

        ; Enabled used interrupts (timer & Keyboard)
        in al, 0x21
        and  al, ~0x0003
        out 0x21, al

        ; habilito SSE
        mov eax, cr0
        and ax, 0xFFFB		; clear coprocessor emulation CR0.EM
        or ax, 0x2			; set coprocessor monitoring  CR0.MP
        mov cr0, eax
        mov eax, cr4
        or ax, 3 << 9		; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
        or eax, 0x80        ; activo Paginas Globales
        mov cr4, eax

        ; deshabilito el cursor del monitor
        mov dx, 0x3D4
        mov al, 0xA
        out dx, al

        inc dx
        mov al, 0x20
        out dx, al

        call    SetScheduler
