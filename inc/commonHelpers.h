#ifndef COMMON_HELPERS_H
#define COMMON_HELPERS_H

typedef char                byte;
typedef unsigned short      word;
typedef unsigned int        dword;
typedef unsigned long long  qword;

#define ERROR_DEFECTO -1
#define EXITO 1

#define BREAKPOINT __asm__("xchg %bx, %bx");

#define NULL    ((void*)0)

#define longLongHexDigits  (16)
#define Get_Hexa_ASCII(x)  (x) > 9 ? (x) + (0x41 - 10) : (x) + 0x30

#define DIGITS_TABLE_SIZE   100

#ifdef DEBUG
#define CONDITIONAL_BREAKPOINT(x)   if(x){BREAKPOINT}
#endif

#define RAM_SIZE 0x1FFFFFFF

#endif