//Dispatcher, es la interfaz para leer y cambiar el estado de las tareas y obtener la proxima tarea que debe ejecutarse

#include "../../../inc/commonHelpers.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/monitorHandler.h"

// Flag de DEBUG definido por consola con gcc atraves del makefile
// si se buildea con "make debug" se agrega al codigo una serie de breakpoints, mensajes en pantalla y teclas de control
// que facilitan ver como se estan ejecutando las tareas

#ifdef DEBUG
#include "../../../inc/keyboardInputHandler.h"
#endif

extern byte Interrupt_Counter;
extern void __setStack__();

extern Task Idle;

__attribute__((section(".uninitialised_data"))) Task* RunningTask = NULL;
__attribute__((section(".uninitialised_data"))) TaskTSS* PreviousContext = NULL;

#ifdef DEBUG
__attribute__((section(".uninitialised_data"))) byte SkipBreakpoints = 0;
#endif

// lo utiliza la tarea que realiza el cambio de contexto de los registros de SIMD para obtener los registros de la tarea actual
__attribute__((section(".kernel32"))) dword* GetNewSimdRegisters(){
    return RunningTask->tss->SimdSpace;
}

// Algoritmo de seleccion de tarea a ejecutar. Lo llama la rutina de interrupcion del system tick.
// El criterio es: todas las tareas tienen como tiempo de ejecucion el tiempo entre su tick de inicio y el de inicio de la siguiente tarea
// Si informan por system call que ya terminaron su ejecucion, y todavia no es tiempo de ejecutar la siguente tarea, se cambia al contexto de la
// tarea IDLE (que no esta en la lista de tareas a ejecutar y nunca se informa como terminada)
// retorna un puntero a la proxima tss o NULL (0) si no hay que cambiar de contexto.
__attribute__((section(".irq_handlers"))) TaskTSS* Dispatch()
{
    int i;
    Task *nextTask = NULL;
    Task* tasksList = GetTasksList();
    #ifdef DEBUG
    byte tickString[4] = "0x  ";
    byte lastKey;
    #endif

    Interrupt_Counter = (Interrupt_Counter+1) % 100;
    for(i=0; i<GetTasksAmount(); i++)
    {
        if(!tasksList[i].count)
        {
            nextTask = tasksList + i;
        }
        tasksList[i].count = (tasksList[i].count+1) % tasksList[i].period;
    }
    #ifdef DEBUG
    GetLastKey(&lastKey);
    if(SkipBreakpoints)
    {
        SkipBreakpoints = !(lastKey == 0x1B); // si aprieto S vuelve a parar en los breakpoints
    }
    else
    {
        SkipBreakpoints = (lastKey == 0x2D); // si aprieto R deja de parar en los breakpoints
    }
    tickString[2] = Get_Hexa_ASCII(((byte)(Interrupt_Counter>>4 & 0xF)));
    tickString[3] = Get_Hexa_ASCII(((byte)Interrupt_Counter & 0xF));
    MonitorPrintAt(tickString, 4, 1980);
    #endif
    if(nextTask != NULL)
    {
        if(RunningTask != NULL)
        {
            RunningTask->status = WAITING;
            PreviousContext = RunningTask->tss;
        }
        #ifdef DEBUG
        MonitorPrintAt(nextTask->ID, 4, 1940);
        CONDITIONAL_BREAKPOINT(!SkipBreakpoints)
        #endif
        nextTask->status = RUNNING;
        nextTask->tss->TaskCounter++;
        RunningTask = nextTask;
        return nextTask->tss;
    }
    else
    {
        if(RunningTask->status == FINISHED)
        {
            nextTask = &Idle;
            #ifdef DEBUG
            MonitorPrintAt(nextTask->ID, 4, 1940);
            CONDITIONAL_BREAKPOINT(!SkipBreakpoints)
            #endif
            nextTask->tss->TaskCounter++;
            PreviousContext = RunningTask->tss;
            RunningTask = nextTask;
            return nextTask->tss;
        }
    }
    #ifdef DEBUG
    CONDITIONAL_BREAKPOINT(!SkipBreakpoints)
    #endif
    return (TaskTSS*)NULL;
}

__attribute__((section(".kernel32"))) void FinishTask()
{
    RunningTask->status = FINISHED;
}
