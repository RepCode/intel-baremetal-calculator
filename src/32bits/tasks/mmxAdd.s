USE32
section .task2_text

EXTERN  NumbersArray2
EXTERN  NumbersArrayIndex2
EXTERN  DisplayedResult2

GLOBAL __mmx_add__

; funcion utilizada para realizar suma saturada en tamano word usando los registros mmx
__mmx_add__:
    mov eax, NumbersArray2
    mov ebx, [NumbersArrayIndex2]
    cmp ebx, 0
    je  .exit
    mov cx, 0
    movq mm0, [eax]
    .loop:
        movq mm1, [eax + 8 + 8 * ecx]
        paddusw mm0, mm1
        add cx, 1
        cmp cx, bx
        jne .loop
    movq [DisplayedResult2], mm0
    .exit:
        ret