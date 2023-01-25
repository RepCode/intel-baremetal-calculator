#include "../../../inc/commonHelpers.h"
#include "../../../inc/sys_calls_api.h"

// tarea IDLE, ejecutada unicamente cuando las tareas todavia tienen una cuota de ejecucion disponible pero ya terminaron con su tarea.
__attribute__((section(".task3_text"))) void IdleTask()
{
    while (1)
    {
        __sys_call_go_hlt__();
    }

    return;
}
