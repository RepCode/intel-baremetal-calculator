USE32

section .task1_bss
PartialSum:  TIMES 2  dq  0

section .task1_text

EXTERN  NumbersArray1
EXTERN  NumbersArrayIndex1
EXTERN  DisplayedResult1

GLOBAL __sse_add__

; funcion utilizada para realizar suma no saturada en tamano quadword
__sse_add__:
    mov eax, NumbersArray1
    mov ebx, [NumbersArrayIndex1]
    cmp ebx, 0
    je  .exit
    mov cx, 0
    movdqu xmm0, [eax]
    .loop:
        movdqu xmm1, [eax + 16 + 8 * ecx]
        paddq xmm0, xmm1
        add cx, 2
        cmp cx, bx
        jle .loop
    movq [PartialSum], xmm0
    movhlps xmm0,xmm0
    movq [PartialSum + 8],xmm0
    movq mm0, [PartialSum]
    movq mm1, [PartialSum + 8]
    paddq mm0, mm1
    movq [DisplayedResult1], mm0
    .exit:
        ret