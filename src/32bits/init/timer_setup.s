USE32
section .kernel32

GLOBAL __PIT_Set_Counter0__

__PIT_Set_Counter0__:
   push ax

   mov al, 00110100b
   out 43h, al    ;En 43h está el registro de control.

   mov ax, 1193   ;Los 3 contadores del PIT reciben una señal de clock 
                           ;de 1.19318[MHz] 
   mul cx         ;1193 * cx / 1.19318[MHz] = 1000 / cx [int/seg]

   out 40h, al; En 40h está el Counter 0.
   mov al, ah
   out 40h, al

   pop ax
   ret
