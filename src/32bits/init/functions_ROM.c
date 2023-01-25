#include "../../../inc/commonHelpers.h"


// funcion en rom para copiar mi codigo de la ROM a la RAM en la inicializacion
__attribute__((section(".functions_rom"))) byte FastMemCopy(const dword *src, dword *dst, dword length)
{
    byte status = (length == 0) ? EXITO : ERROR_DEFECTO;

    if(length > 0)
    {
        while(length)
        {
            length--;
            *dst++ = *src++;
        }
        status = EXITO;
    }

    return(status);
}