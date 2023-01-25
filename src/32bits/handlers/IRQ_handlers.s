; En este archivo se definen todas las rutinas de interrupcion utilizadas

USE32
section .irq_handlers

%include 'asmSharedValues.s'

EXTERN  Keyboard_Buffer
EXTERN  Keyboard_Write_Index
EXTERN  Keyboard_Is_Release
EXTERN  AddNewPage
EXTERN  Dispatch
EXTERN  NewStack
EXTERN  RunningTask
EXTERN  PreviousContext
EXTERN  CurrentSimdRegisters
EXTERN  GetNewSimdRegisters
EXTERN  FinishTask
EXTERN  MonitorPrintAt
EXTERN  GetLastKey
EXTERN  AddNumberToTable
EXTERN  ReadDigitsTable
EXTERN _idtr
EXTERN PrintDevideZeroError
EXTERN PrintGPError
EXTERN PrintUDError
EXTERN PrintDFError


GLOBAL  __interrupHandler__0
GLOBAL  __interrupHandlerend__0
GLOBAL  __devide_zeroIRQ__
GLOBAL  __general_protectionIRQ__
GLOBAL  __invalid_opcodeIRQ__
GLOBAL  __double_faultIRQ__
GLOBAL  __keyboardIRQ__
GLOBAL  __systickIRQ__
GLOBAL  __pageFaultIRQ__
GLOBAL  __deviceNotAvailable
GLOBAL  __sys_call

%define go_hlt                  0
%define finish_task             1
%define monitor_print_at        2
%define get_last_key_pressed    3
%define add_number              4
%define get_digits              5
%define read_byte               6
%define make_df                 7

; aprovecho las macros de nasm para crear 1 funcion de igual tamano para cada exepcion (solo varian en el numero que se pone en dx indicando que
; excepcion es)
%assign i 0
%rep    256
__interrupHandler__%+i:
        xchg bx, bx
        mov dx, i
        hlt
__interrupHandlerend__%+i:
%assign i i+1 
%endrep

; Estas 4 funciones generan errores en pantalla para probar el cacheo de excepciones en algunos escenarios (division por cero, general protection,
; invalid opcode y double fault) las triggerea la tarea del teclado
__devide_zeroIRQ__:
    call PrintDevideZeroError
    xchg bx, bx
    mov dx, 0
    cli
    hlt

__general_protectionIRQ__:
    call PrintGPError
    xchg bx, bx
    mov dx, 0xD
    cli
    hlt

__invalid_opcodeIRQ__:
    call PrintUDError
    xchg bx, bx
    mov dx, 0x6
    cli
    hlt

__double_faultIRQ__:
    call PrintDFError
    xchg bx, bx
    mov dx, 0x6
    cli
    hlt

; interrupcion del system tick. Encargada del cambio de tareas
__systickIRQ__:
    cli
    ; guardo todos los registros ya que potencialmente voy a cambiar de tarea
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi

    push    ebp
    mov     ebp, esp
    ; llamo al dispatcher que determina si hay que cambiar de tarea
    call Dispatch; retorna un puntero a la proxima tss o 0 si no hay que cambiar
    
    mov     esp, ebp
    pop     ebp

    cmp dword [PreviousContext], 0 ; Si es el primero no tengo que salvar el contexto actual
    jne .continue

.first:
    pop    edi
    pop    esi
    pop    edx
    pop    ecx
    pop    ebx
    pop    ebx
    cmp eax, 0 ; si no hay que cambiar de tarea
    je  .exit
    jmp .load_new_context

.continue:
    cmp eax, 0; si no hay que cambiar de tarea
    je  .clean_stack

.save_context:
    pop    edi
    pop    esi
    pop    edx
    pop    ecx
    pop    ebx

    push eax; salvo el proximo contexto

    mov eax, [PreviousContext]; busco el tss del contexto actual

    mov [eax + 0], ebx
    mov [eax + 4], ecx
    mov [eax + 8], edx

    pop ecx; vuelvo a cargar el proximo contexto
    pop ebx; cargo el eax de la tarea previa en ebx para guardarlo

    mov [eax + 12], ebx
    mov [eax + 16], esi
    mov [eax + 20], edi
    mov [eax + 24], ebp
    mov [eax + 32], gs
    mov [eax + 36], fs
    mov [eax + 44], es
    mov [eax + 48], ds
    mov ebx, cr3
    mov [eax + 52], ebx
    mov ebx, [esp]
    mov [eax + 68], ebx
    mov ebx, [esp + 4]
    mov [eax + 56], ebx
    mov ebx, [esp + 8]
    mov [eax + 72], ebx
    mov [eax + 60], esp
    mov [eax + 64], ss

    mov eax, ecx; guardo el proximo contexto en eax para .load_new_context

.load_new_context:
    mov edx, [eax + 52]
    mov cr3, edx

    mov ebx, [eax + 0]
    mov ecx, [eax + 4]
    mov edx, [eax + 16]
    mov esi, edx
    mov edx, [eax + 20]
    mov edi, edx
    mov edx, [eax + 24]
    mov ebp, edx
    mov edx, [eax + 32]
    mov gs, edx
    mov edx, [eax + 36]
    mov fs, edx
    mov edx, [eax + 44]
    mov es, edx
    mov edx, [eax + 48]
    mov ds, edx

    mov edx, [eax + 60]
    mov[NewStack], edx
    mov edx, [eax + 64]
    mov[NewStack+4], edx

    ; seteo la nueva pila
    lss esp, [NewStack]

    ; marco que hubo un task switch
    push    eax
    smsw    ax
    or      ax, 0x8
    lmsw    ax
    pop     eax

    ; veo si es la primera vez que cargo la tarea
    cmp byte [eax + 80], 0x0 ; en 80 esta el flag loaded de la tarea
    jne .finish
    mov byte [eax + 80], 0x1 ; la marco como loaded
    mov edx, [eax + 40] ; SS3
    add edx, 3
    push    edx
    mov edx, [eax + 28] ; ESP3
    push    edx
    mov edx, [eax + 72] ; EFLAGS
    push    edx
    mov edx, [eax + 56] ; CS
    or  edx, 0x3
    push    edx
    mov edx, [eax + 68] ; EIP
    push    edx
.finish:
    mov edx, [eax + 8]
    mov eax, [eax + 12]

    jmp .exit

.clean_stack: ; se ejecuta cuando no hay que cambiar contexto para limpiar lo que se pusheo al inicio en la pila
    pop    edi
    pop    esi
    pop    edx
    pop    ecx
    pop    ebx
    pop    eax

.exit:
    push    eax
    mov al, 0x20
    out 0x20, al
    pop    eax
    sti
    iret

; rutina del teclado. Interrupte cuando se apreta o suelta una tecla
; para esta implementacion solo me interesa cuando se pulsa por lo que se ignora cuando se suelta.
; La rutina solo se encarga de cargar en un buffer circular la tecla precionada (no importa cual)
; Luego las tareas le pueden dar uso a las teclas (en este caso solo la keynoardTask lee las teclas por system call)
__keyboardIRQ__:
    push    eax
    push    ebx
    push    ecx
    in  al, 0x60
    mov cl, byte [Keyboard_Is_Release]
    cmp cl, 0x01
    jne .processKey
    mov cl, 0x00
    mov [Keyboard_Is_Release], byte cl
    jmp .KeyboardIRQExit
    .processKey:
    cmp al, 0xf0 ; veo si es un comando de release
    jne .saveKey
    mov al, 1
    mov [Keyboard_Is_Release], byte al
    jmp  .KeyboardIRQExit
    .saveKey:
        xor cx, cx
        mov cl, byte [Keyboard_Write_Index]
        mov ebx, Keyboard_Buffer
        add bx, cx
        mov [ebx], al
        inc cx
        cmp cx, (9-1)
        jna .updateIndex
        mov cx, 0x00
    .updateIndex:
        mov [Keyboard_Write_Index], byte cl
    .KeyboardIRQExit:
        mov al, 0x20
        out 0x20, al
        pop    ecx
        pop    ebx
        pop    eax
        iret


; rutina de interrupcion por page fault. hace pop del error coda y se fija si el motivo de error es
; por una pagina no mapeada entonces la intenta agregar (le cede el trabajo de agregarla al handler de paginacion en pagingHandler.c)
__pageFaultIRQ__:
    push eax
    push edx
    mov edx, [esp+16]
    mov eax, 0x01
    and eax, edx
    cmp eax, 0x01
    je  .exit
    mov eax, cr2
    push eax
    call AddNewPage; intenta agregar la pagina (solo tareas habilitadas para paginar, es decir que tengan su paginas paginadas lograran hacerlo)
    pop eax        ; Asi, si es un page fault esperado (la tarea 1 queriendo leer memoria por syscall) no habra error, pero si lo habra en un contexto no habilitado para hacerlo
    .exit:
    pop edx
    pop eax
    add esp, 4
    iret

; handler para hacer el context switch de los registros de SIMD
__deviceNotAvailable:
    clts
    push eax
    cmp dword [CurrentSimdRegisters], 0
    je  .first
    mov eax, [CurrentSimdRegisters] 
    fxsave  [eax]
    call GetNewSimdRegisters ; le pido al scheduler que me diga el contexto nuevo en base a la tarea que esta ejecutandoce cuando salta la interrupcion
    fxrstor [eax]
    mov [CurrentSimdRegisters], eax
    jmp .exit
.first:
    call GetNewSimdRegisters
    mov [CurrentSimdRegisters], eax
.exit:
    pop eax
    iret

; system call, implementada en la int 0x80. Todas las llamadas las hacen las tareas a traves de la sys calls api
__sys_call:
    cmp eax, go_hlt ; para ir hlt. Solo la usa la tarea IDLE que nunca informa haber terminado su ciclo (relacionado con el algoritmo de scheduling utilizado)
    jne .check_finished
    hlt
    jmp .exit
.check_finished:
    cmp eax, finish_task ; lo utilizan las tareas para avisarle al scheduler que terminaron y, posterior a eso ir a hlt.
    jne .check_monitor_at
    call FinishTask
    hlt
    jmp .exit
.check_monitor_at:
    cmp eax, monitor_print_at ; utilizado para imprimir en pantalla
    jne .check_get_last_key
    push ebx
    push ecx
    push edx

    call    MonitorPrintAt

    pop edx
    pop ecx
    pop ebx
    jmp .exit
.check_get_last_key: ; utilizado para obtener la ultima tecla precionada por el teclado (devuelve la ultima tecla del buffer circular)
    cmp eax, get_last_key_pressed
    jne .chec_add_number
    push ebx

    call    GetLastKey

    pop ebx
    jmp .exit
.chec_add_number: ; utilizada para agregar un numero de 64 bits a la tabla de digitos
    cmp eax, add_number
    jne .check_get_digits
    push ebx

    call    AddNumberToTable

    pop ebx
    jmp .exit
.check_get_digits: ; utilizada para leer datos de la tabla de digitos
    cmp eax, get_digits
    jne .check_read_byte

    push ebx
    push ecx

    call    ReadDigitsTable

    pop ecx
    pop ebx
    jmp .exit
.check_read_byte:           ; utilizada para leer un bit en alguna posicion de memoria determinada. Asi por ejemplo la tarea 1 puede leer el bit 0 (paginado a las rutinas de interrupcion)
    cmp eax, read_byte      ; sin que haya un error de privilegio y sin tener que bajarle privilegios a la pagina
    jne .check_df
    mov byte eax, [ebx]
.check_df:  ; utilizada para generar la double fault. Borra el descriptor de division por cero y genera una excepcion de division por cero.
    cmp eax, make_df
    jne .exit
    mov eax, [_idtr+2]
    mov dword [eax], 0x0 ; borro de la idt el descriptor de la division por 0
    mov eax, 0x0
    div eax

.exit:
    iret
