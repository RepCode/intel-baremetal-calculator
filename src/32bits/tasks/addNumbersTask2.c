#include "../../../inc/commonHelpers.h"
#include "../../../inc/keyboardTask.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/sys_calls_api.h"

#define PRINT_START 1080

extern void __mmx_add__();

qword DisplayedResult2 __attribute__((section(".task2_bss")));
qword NumbersArray2[DIGITS_TABLE_SIZE] __attribute__((section(".task2_bss")));
dword NumbersArrayIndex2 __attribute__((section(".task2_bss")));
static byte NumberText[longLongHexDigits] __attribute__((section(".task2_bss")));

static byte ResultMessage[19] __attribute__((section(".task2_data"))) = " El resultado 2 es:";

// tarea 2: suma saturada en tamano word
__attribute__((section(".task2_text"))) void AddNumbersTask2()
{
    dword i;
    byte digit;

    // imprimo los caracteres que van a quedar fijos en la pantalla
    __sys_call_monitor_print_at__(ResultMessage, 19, PRINT_START);

    while (1)
    {
        // obtengo los valores en la tabla de digitos a traves de la system call correspondiente
        NumbersArrayIndex2 = __sys_call_get_digits__(NumbersArray2, DIGITS_TABLE_SIZE*sizeof(qword));
        DisplayedResult2 = 0;
        __mmx_add__(); // llamo a la rutina de suma SIMD
        for(i=0; i<longLongHexDigits; i++)
        {
            digit = (DisplayedResult2 >> (i*4)) & 0xF;
            NumberText[longLongHexDigits-i-1] = Get_Hexa_ASCII(digit);
        }
        __sys_call_monitor_print_at__(NumberText, longLongHexDigits, PRINT_START+161);
        
        // le informo el fin de ejecucion al kernel, que marca la tarea finished y se va a hlt.
        __sys_call_finish_task__();
    }

    return;
}