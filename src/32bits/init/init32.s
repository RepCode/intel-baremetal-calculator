;    Inicializacion estando ya en modo protegido. Realiza todos los memcopy necesarios de ROM a RAM y luego llama a los handlers de inicializacion
;    necesarios (Descriptores, Paginas, PIC, etc) todo previo al salto a la inicializacion del kernel
USE32
section .start32

EXTERN  DS_SEL
EXTERN  setGlobalDescriptionTables
EXTERN  __STACK_END_32
EXTERN  __STACK_SIZE_32
EXTERN  FastMemCopy
EXTERN  kernel32_code_size
EXTERN  __KERNEL_32_PHY
EXTERN  __KERNEL_32_LMA
EXTERN  __codigo_kernel32_size
EXTERN  __setDescriptorTables__
EXTERN  __codigo_set_descriptor_tables_size
EXTERN  __SET_DESCTIPTOR_TABLES_VMA
EXTERN  __SET_DESCTIPTOR_TABLES_LMA
EXTERN  __initialised_data_size
EXTERN  __INITIALISED_PHY
EXTERN  __INITIALISED_LMA
EXTERN  __functions_size
EXTERN  __FUNCTIONS_VMA
EXTERN  __FUNCTIONS_LMA
EXTERN  Keyboard_Setup

EXTERN  __sys_calls_api_size
EXTERN  __SYS_CALLS_API_PHY
EXTERN  __SYS_CALLS_API_LMA

EXTERN  __task1_text_size
EXTERN  __TASK1_TEXT_PHY
EXTERN  __TASK1_TEXT_LMA
EXTERN  __task1_data_size
EXTERN  __TASK1_DATA_PHY
EXTERN  __TASK1_DATA_LMA

EXTERN  __task2_text_size
EXTERN  __TASK2_TEXT_PHY
EXTERN  __TASK2_TEXT_LMA
EXTERN  __task2_data_size
EXTERN  __TASK2_DATA_PHY
EXTERN  __TASK2_DATA_LMA

EXTERN  __task0_text_size
EXTERN  __TASK0_TEXT_PHY
EXTERN  __TASK0_TEXT_LMA
EXTERN  __task0_data_size
EXTERN  __TASK0_DATA_PHY
EXTERN  __TASK0_DATA_LMA

EXTERN  __task3_text_size
EXTERN  __TASK3_TEXT_PHY
EXTERN  __TASK3_TEXT_LMA
EXTERN  __task3_data_size
EXTERN  __TASK3_DATA_PHY
EXTERN  __TASK3_DATA_LMA

EXTERN  __CS_SEL_32_INDEX
EXTERN  __kernel32_init__
EXTERN  GetBootPageDirectory
EXTERN  SetBootPages
EXTERN  __irq_handlers_size
EXTERN  __IRQ_HANDLERS_VMA
EXTERN  __IRQ_HANDLERS_LMA

EXTERN  __BASIC_TSS
EXTERN  __TASK_KERNEL_STACK_END_VMA
EXTERN  __DS_SEL_32_INDEX
EXTERN  __TSS_SELL_INDEX

GLOBAL __start32_launcher__

%define X86_CR0_PG 0x80000001

__start32_launcher__:

    %ifdef DEBUG 
        xchg bx, bx
    %endif

    ;inicializar selectores de datos
    mov ax, DS_SEL
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    ;inicializar pila
    mov ss, ax
    mov esp, __STACK_END_32
    xor eax, eax
    ;limpio la pila
    mov ecx, __STACK_SIZE_32
    .stack_init:
        push eax
        loop .stack_init
    mov esp, __STACK_END_32
    
    push ebp; copy kernel
    mov ebp, esp
    push __codigo_kernel32_size
    push __KERNEL_32_PHY
    push __KERNEL_32_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy kernel
    mov ebp, esp
    push __initialised_data_size
    push __INITIALISED_PHY
    push __INITIALISED_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy kernel
    mov ebp, esp
    push __codigo_set_descriptor_tables_size
    push __SET_DESCTIPTOR_TABLES_VMA
    push __SET_DESCTIPTOR_TABLES_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy kernel
    mov ebp, esp
    push __functions_size
    push __FUNCTIONS_VMA
    push __FUNCTIONS_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy IRQ handlers
    mov ebp, esp
    push __irq_handlers_size
    push __IRQ_HANDLERS_VMA
    push __IRQ_HANDLERS_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task1
    mov ebp, esp
    push __sys_calls_api_size
    push __SYS_CALLS_API_PHY
    push __SYS_CALLS_API_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task1
    mov ebp, esp
    push __task1_text_size
    push __TASK1_TEXT_PHY
    push __TASK1_TEXT_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task1
    mov ebp, esp
    push __task1_data_size
    push __TASK1_DATA_PHY
    push __TASK1_DATA_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard    

    push ebp; copy task2
    mov ebp, esp
    push __task2_text_size
    push __TASK2_TEXT_PHY
    push __TASK2_TEXT_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task2
    mov ebp, esp
    push __task2_data_size
    push __TASK2_DATA_PHY
    push __TASK2_DATA_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task0
    mov ebp, esp
    push __task0_text_size
    push __TASK0_TEXT_PHY
    push __TASK0_TEXT_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task0
    mov ebp, esp
    push __task0_data_size
    push __TASK0_DATA_PHY
    push __TASK0_DATA_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task3
    mov ebp, esp
    push __task3_text_size
    push __TASK3_TEXT_PHY
    push __TASK3_TEXT_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    push ebp; copy task3
    mov ebp, esp
    push __task3_data_size
    push __TASK3_DATA_PHY
    push __TASK3_DATA_LMA
    call FastMemCopy
    leave
    cmp eax, 1 ; chequeo el return de memcpy
    jne .guard

    ; llamo a la rutina de inicializacion de los descriptores que va a utilizar el sistema operativo (despues de esto dejo de utilizar la GDT basica)
    push    ebp
    mov     ebp, esp
    call __setDescriptorTables__

    ; Seteo cr3 al inicio de la DPT
    push    ebp
    mov     ebp, esp
    call    GetBootPageDirectory
    mov     cr3, eax

    push    ebp
    mov     ebp, esp
    call SetBootPages

    %ifdef DEBUG 
        xchg bx, bx
    %endif

    ; Activo paginacion
    mov eax, cr0
    or  eax, X86_CR0_PG

    mov cr0, eax

    ; seteo la TSS basica para el cambio de contexto con niveles de privilegio
    mov eax, __BASIC_TSS
    mov dword [eax + 4], __TASK_KERNEL_STACK_END_VMA
    mov dword [eax + 8], __DS_SEL_32_INDEX

    mov ax, __TSS_SELL_INDEX
    ltr ax

    jmp __CS_SEL_32_INDEX:__kernel32_init__

.guard:
    hlt
