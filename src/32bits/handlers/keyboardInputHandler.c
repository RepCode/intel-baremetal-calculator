/*
    engloba funciones para obtener la ultima tecla incresada al teclado, leer los inputs de la tabla de digitos y agregar inputs a la tabla de digitos.
*/

#include "../../../inc/keyboardInputHandler.h"

extern dword DigitsTableIndex;
extern byte Keyboard_Buffer[9]; // guardo las teclas en un buffer como precaución de que se pueda ingreasar más de una en una ventana de lectura (aunque no sucede en este caso)
extern byte Keyboard_Write_Index;

qword g_Digits_tables[DIGITS_TABLE_SIZE] __attribute__((section(".digits_table")));

__attribute__((section(".kernel32"))) dword GetLastKey(byte* key)
{
    *key = Keyboard_Buffer[Keyboard_Write_Index ? (dword)(Keyboard_Write_Index-1) : 9-1];
    return Keyboard_Write_Index;
}

__attribute__((section(".kernel32"))) void AddNumberToTable(qword* number)
{
    g_Digits_tables[DigitsTableIndex] = *number;
    DigitsTableIndex = (DigitsTableIndex+1) % DIGITS_TABLE_SIZE;
    return;
}

__attribute__((section(".kernel32"))) dword ReadDigitsTable(void* buffer, dword sizeBytes)
{
    dword i;
    byte* pointer = (byte*) buffer;
    sizeBytes = (DigitsTableIndex*sizeof(qword)) < sizeBytes ? (DigitsTableIndex*sizeof(qword)) : sizeBytes;
    for(i=0; i<sizeBytes; i++)
    {
        *(pointer+i) = (byte)*(((byte*)g_Digits_tables) + i);
    }
    return DigitsTableIndex;
}
