#include "../../../inc/commonHelpers.h"
#include "../../../inc/keyboardTask.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/sys_calls_api.h"

#define PRINT_START 1576

byte Keyboard_Read_Index __attribute__((section(".task0_bss")));
static byte NumberText[longLongHexDigits] __attribute__((section(".task0_bss")));

static byte InputText[7] __attribute__((section(".task0_data"))) = "Input: ";

extern dword DigitsTableIndex;

static char getNumber(char key);

// tarea encargada de leer la ultima tecla ingresada en el teclado y accionar acorde a ello.
// si es un caracter hexadecimal, va formando un numero de 64bits (de forma circular), si es un enter guarda dicho numero en la tabla
// de digitos y si es una tecla utilizada para generar exepciones genera dicha excepcion.
__attribute__((section(".task0_text"))) void ReadKeyboardBuffer()
{
    byte key;
    dword currentIndex;
    qword currentNumber = 0;
    dword i;
    byte digit;
    __sys_call_monitor_print_at__(InputText, 7, PRINT_START);
    while (1)
    {
        currentIndex = __sys_call_get_last_key__(&key);
        if(Keyboard_Read_Index != currentIndex)
        {
            Keyboard_Read_Index = currentIndex;
            if((key == 0x5a)) // si es un ENTER
            {
                __sys_call_add_number__(&currentNumber);
                currentNumber = 0;
            }
            else if((key == 0x44)) // si es una O
            {
                asm("int $0x00");
            }
            else if((key == 0x35)) // si es una Y
            {
                key = 0;
                key = 1 / key;
            }
            else if((key == 0x3C)) // si es una U
            {
                asm("mov %%cr6, %0" : "=r" (key));
            }
            else if((key == 0x43)) // si es una I
            {
                __sys_call_make_double_fault__();
            }
            key = getNumber(key);
            if(key != -1) // si es un numero
            {
                currentNumber = (currentNumber << 4) + key;
            }
        }
        for(i=0; i<longLongHexDigits; i++)
        {
            digit = (currentNumber >> (i*4)) & 0xF;
            NumberText[longLongHexDigits-i-1] = Get_Hexa_ASCII(digit);
        }
        __sys_call_monitor_print_at__(NumberText, longLongHexDigits, PRINT_START+7);

        __sys_call_finish_task__(); // la marco como terminada y voy a hlt
    }

    return;
}

// busco si la tecla se corresponde con algun caracter hexadecimal
__attribute__((section(".task0_text"))) static char getNumber(char key)
{
    if(key == 0x70 || key == 0x45)
    {
        return 0;
    }
    if(key == 0x69 || key == 0x16)
    {
        return 1;
    }
    if(key == 0x72 || key == 0x1E)
    {
        return 2;
    }
    if(key == 0x7A || key == 0x26)
    {
        return 3;
    }
    if(key == 0x6B || key == 0x25)
    {
        return 4;
    }
    if(key == 0x73 || key == 0x2E)
    {
        return 5;
    }
    if(key == 0x74 || key == 0x36)
    {
        return 6;
    }
    if(key == 0x6C || key == 0x3D)
    {
        return 7;
    }
    if(key == 0x75 || key == 0x3E)
    {
        return 8;
    }
    if(key == 0x7D || key == 0x46)
    {
        return 9;
    }
    if(key == 0x1C)
    {
        return 0xA;
    }
    if(key == 0x32)
    {
        return 0xB;
    }
    if(key == 0x21)
    {
        return 0xC;
    }
    if(key == 0x23)
    {
        return 0xD;
    }
    if(key == 0x24)
    {
        return 0xE;
    }
    if(key == 0x2B)
    {
        return 0xF;
    }
    return -1;
}