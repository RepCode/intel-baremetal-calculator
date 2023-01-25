#include "../../../inc/commonHelpers.h"
#include "../../../inc/keyboardTask.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/sys_calls_api.h"

#define PRINT_START 1040

extern void __sse_add__();

qword DisplayedResult1 __attribute__((section(".task1_bss")));
qword NumbersArray1[DIGITS_TABLE_SIZE] __attribute__((section(".task1_bss")));
dword NumbersArrayIndex1 __attribute__((section(".task1_bss")));
static byte NumberText[longLongHexDigits] __attribute__((section(".task1_bss")));

static byte ResultMessage[19] __attribute__((section(".task1_data"))) = " El resultado 1 es:";
static byte MemoryReadMessage[33] __attribute__((section(".task1_data"))) = "Valor en memoria del resultado 1:";

// tarea 1: suma no saturada en tamano quadword, que ademas lee la posicion de memoria apuntada por el valor de la suma
__attribute__((section(".task1_text"))) void AddNumbersTask()
{
    dword i;
    byte digit;

    // imprimo los caracteres que van a quedar fijos en la pantalla
    __sys_call_monitor_print_at__(ResultMessage, 19, PRINT_START);
    __sys_call_monitor_print_at__(MemoryReadMessage, 33, PRINT_START+321);

    while (1)
    {
        // obtengo los valores en la tabla de digitos a traves de la system call correspondiente
        NumbersArrayIndex1 = __sys_call_get_digits__(NumbersArray1, DIGITS_TABLE_SIZE*sizeof(qword));
        DisplayedResult1 = 0;
        __sse_add__(); // llamo a la rutina de suma SIMD
        for(i=0; i<longLongHexDigits; i++)
        {
            digit = (DisplayedResult1 >> (i*4)) & 0xF;
            NumberText[longLongHexDigits-i-1] = Get_Hexa_ASCII(digit);
        }
        __sys_call_monitor_print_at__(NumberText, longLongHexDigits, PRINT_START + 161);

        // Si el resultado está dentro del rango de memoria, lo leo a traves de la system call
        if(DisplayedResult1 < RAM_SIZE)
        {
            digit = __sys_call_read_byte__((dword)DisplayedResult1);
            NumberText[1] = Get_Hexa_ASCII((digit & 0xF));
            NumberText[0] = Get_Hexa_ASCII((digit>>4) & 0xF);
            __sys_call_monitor_print_at__(NumberText, 2, PRINT_START+355);
        }
        else
        {
            NumberText[0] = '-';
            NumberText[1] = '-';
            __sys_call_monitor_print_at__(NumberText, 2, PRINT_START+355);
        }
        // le informo el fin de ejecucion al kernel, que marca la tarea finished y se va a hlt.
        __sys_call_finish_task__();
    }

    return;
}