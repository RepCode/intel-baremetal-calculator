/*
    Engloba todo lo relacionao a imprimir en pantalla. Provee funcionalidades para imprimir errores, para escribir de a lineas (manteniendo un index)
    e imprimir en un index en particular.
*/

#include "../../../inc/commonHelpers.h"
#include "../../../inc/monitorHandler.h"

extern void* MONITOR_RAM_START_VMA;

#define TableStart      ((byte*)&MONITOR_RAM_START_VMA)
#define LineSize        160
#define RamSize         4000

static void PrintCriticalError(byte* message);

static dword CurrentIndex __attribute__((section(".uninitialised_data"))) = 0;
static byte CurrentColor __attribute__((section(".initialised_data"))) = TEXT_COLOR;

// mensajes de error para las fallas generadas a proposito por teclado
static byte DevideZeroErrorMessage[80] __attribute__((section(".initialised_data"))) = "                          Error de division por cero :(                         ";
static byte GPErrorMessage[80] __attribute__((section(".initialised_data"))) = "                         Falla de proteccion general :(                         ";
static byte UDErrorMessage[80] __attribute__((section(".initialised_data"))) = "                         Codigo de operacion invalido :(                        ";
static byte DFErrorMessage[80] __attribute__((section(".initialised_data"))) = "                                   Doble Falta :(                               ";

__attribute__((section(".kernel32"))) void MonitorPrint(byte* string, dword length, dword type)
{
    dword i;
    for(i=0; i < length; i++)
    {
        TableStart[CurrentIndex++] = string[i];
        TableStart[CurrentIndex] = CurrentColor;
        CurrentIndex = (CurrentIndex+1) % RamSize;
    }
    if(type == JUMP_LINE)
    {
        MonitorJumpLine();
    }

    return;
}

// index goes from 0 to 2000 for every possible character
inline __attribute__((section(".kernel32"))) void MonitorJumpLine()
{
    CurrentIndex = ((dword)(CurrentIndex / LineSize) + 1) * LineSize;
}

// index goes from 0 to 2000 for every possible character
__attribute__((section(".kernel32"))) void MonitorPrintAt(byte* string, dword length, dword index)
{
    dword i;
    index = index * 2;
    for(i=0; i < length; i++)
    {
        TableStart[index++] = string[i];
        TableStart[index] = CurrentColor;
        index = (index+1) % RamSize;
    }

    return;
}

__attribute__((section(".kernel32")))inline void PrintDevideZeroError()
{
    PrintCriticalError(DevideZeroErrorMessage);
}

__attribute__((section(".kernel32")))inline void PrintGPError()
{
    PrintCriticalError(GPErrorMessage);
}

__attribute__((section(".kernel32")))inline void PrintUDError()
{
    PrintCriticalError(UDErrorMessage);
}


__attribute__((section(".kernel32")))inline void PrintDFError()
{
    PrintCriticalError(DFErrorMessage);
}

__attribute__((section(".kernel32"))) static void PrintCriticalError(byte* message)
{
    dword i;
    CurrentColor = ERROR_TEXT_COLOR;
    for(i=0; i<RamSize; i+=2)
    {
        TableStart[i] = ' ';
        TableStart[i+1] = CurrentColor;
    }

    MonitorPrintAt(message, 80, 960);
}