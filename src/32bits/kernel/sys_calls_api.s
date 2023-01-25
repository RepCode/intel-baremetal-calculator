; API implementada para facilitarle las system calls a las tareas. Reciben por ABI los parametros y luego
; guardan resguardan los registros, los setean acorde al contrado con la int 0x80, realizan la interrupcion
; y recuperan los registros

USE32
section .sys_calls_api

%include 'asmSharedValues.s'

GLOBAL  __sys_call_finish_task__
GLOBAL  __sys_call_go_hlt__
GLOBAL  __sys_call_monitor_print_at__
GLOBAL  __sys_call_get_last_key__
GLOBAL  __sys_call_add_number__
GLOBAL  __sys_call_get_digits__
GLOBAL  __sys_call_read_byte__
GLOBAL  __sys_call_make_double_fault__

__sys_call_finish_task__:
    push    eax
    mov eax,  finish_task
    int 0x80
    pop eax
    ret

__sys_call_go_hlt__:
    push    eax
    mov eax,  go_hlt
    int 0x80
    pop eax

    ret

__sys_call_monitor_print_at__:
    push    eax
    push    ebx
    push    ecx
    push    edx

    mov eax,  monitor_print_at
    mov ebx, [esp+28]
    mov ecx, [esp+24]
    mov edx, [esp+20]
    int 0x80

    pop    edx
    pop    ecx
    pop    ebx
    pop    eax

    ret

__sys_call_get_last_key__:
    push    ebx

    mov ebx, [esp + 8]; guardo el puntero donde va el char

    mov eax,  get_last_key_pressed
    int 0x80

    pop    ebx

    ret

__sys_call_add_number__:
    push    ebx    
    mov ebx, [esp + 8]

    mov eax,  add_number
    int 0x80
    pop    ebx

    ret

__sys_call_get_digits__:
    push    ebx
    push    ecx
    mov ebx, [esp + 16]
    mov ecx, [esp + 12]

    mov eax,  get_digits
    int 0x80

    pop    ecx
    pop    ebx

    ret

__sys_call_read_byte__:
    push    ebx    
    mov ebx, [esp + 8]

    mov eax,  read_byte
    int 0x80
    pop    ebx

    ret

__sys_call_make_double_fault__:
    push    eax
    mov eax,  make_df
    int 0x80
    pop eax
    ret    
