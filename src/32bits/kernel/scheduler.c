/*
    El schedules es estatico, inicializa una lista de 3 tareas y la tarea IDLE en una variable aparte definiendo sus contextos de ejecucion,
    Frecuencia, tick de inicio y tarea a correr. Este archivo ademas imprime en pantalla todos los caracteres fijos que no estan relacionados a ninguna tarea.
*/

#include "../../../inc/commonHelpers.h"
#include "../../../inc/keyboardTask.h"
#include "../../../inc/monitorHandler.h"
#include "../../../inc/addNumbersTask.h"
#include "../../../inc/addNumbersTask2.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/idleTask.h"
#include "../../../inc/scheduler.h"
#include "../../../inc/pagingHandler.h"

extern byte WelcomeLine0;
extern byte WelcomeLine1;
extern byte WelcomeLine2;
extern byte WelcomeLine3;
extern byte WelcomeLine4;
extern byte WelcomeLine5;
extern byte WelcomeLine6;
extern byte DevideLine;
extern byte InstructionsLine;
extern byte DebugLine;
extern byte ExceptionsInstructions;

extern  dword  Task1_SIMD_Registers;
extern  dword  Task2_SIMD_Registers;

static void InitScheduler();
static void AddTaskToScheduler(Task task);

static __attribute__((section(".uninitialised_data")))TaskTSS task0Tss;
static __attribute__((section(".uninitialised_data")))TaskTSS task1Tss;
static __attribute__((section(".uninitialised_data")))TaskTSS task2Tss;
static __attribute__((section(".uninitialised_data")))TaskTSS task3Tss;

__attribute__((section(".initialised_data")))Task Idle = { .Run = IdleTask, .tss = &task3Tss, .ID = "IDLE" };

Scheduler scheduler __attribute__((section(".uninitialised_data")));

#ifdef DEBUG
__attribute__((section(".initialised_data"))) byte CurrentTaskMessage[20] = "Tarea en ejecucion: ";
__attribute__((section(".initialised_data"))) byte CurrentTickMessage[20] = "Valor Tick Counter: ";
#endif

__attribute__((section(".kernel32"))) void SetScheduler()
{
    // imprimo en pantalla los textos que quedan fijos (y no se relacionan a las tareas)
    MonitorPrint(&WelcomeLine0, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine1, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine2, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine3, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine4, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine5, 80, DONT_JUMP_LINE);
    MonitorPrint(&WelcomeLine6, 80, JUMP_LINE);
    MonitorPrint(&DevideLine,   80, DONT_JUMP_LINE);
    MonitorPrint(&InstructionsLine, 80, DONT_JUMP_LINE);
    #ifdef DEBUG
    MonitorPrint(&DebugLine, 80, DONT_JUMP_LINE);
    MonitorPrintAt(CurrentTaskMessage, 20, 1920);
    MonitorPrintAt(CurrentTickMessage, 20, 1960);
    #endif
    MonitorPrint(&DevideLine,   80, DONT_JUMP_LINE);
    MonitorPrintAt(&DevideLine,   80, 1600);
    MonitorPrintAt(&ExceptionsInstructions, 80, 1760);
    InitScheduler();
    asm("sti");
    while (1)
    {
        asm("hlt");
    }
    return;
}

// carga la lista de tareas a ejecutar por el dispatcher
__attribute__((section(".kernel32"))) static void InitScheduler()
{
    Task task0 = { .period=20, .Run = ReadKeyboardBuffer, .count=5, .tss = &task0Tss, .ID = "KeyR" };
    Task task1 = { .period=10, .Run = AddNumbersTask, .count=0, .tss = &task1Tss, .ID = "ADD1" };
    Task task2 = { .period=20, .Run = AddNumbersTask2, .count=15, .tss = &task2Tss, .ID = "ADD2" };
    
    // inicializo la tss(una esctructura propia distinta a la de Intel) de cada tarea
    InitTSS(task0.tss, task0.Run, NULL);
    InitTSS(task1.tss, task1.Run, &Task1_SIMD_Registers);
    InitTSS(task2.tss, task2.Run, &Task2_SIMD_Registers);
    InitTSS(Idle.tss, Idle.Run, NULL);

    // indico los contextos de SIMD que van a estar en ejecucion
    AddSimdSection((dword*)&__TASK1_SIMD_VMA, (dword*)&__TASK1_SIMD_PHY);
    AddSimdSection((dword*)&__TASK2_SIMD_VMA, (dword*)&__TASK2_SIMD_PHY);

    // inicializo los directorios de cada tarea con las paginas minimas necesarias para los recursos que van a utilizar las tareas
    task0.tss->CR3 = GetCR3(PAG_PCD_YES, PAG_PWT_YES);
    SetTaskDirectoryPages((dword*)task0.tss->CR3, (dword*)&__TASK0_TEXT_VMA, (dword*)&__TASK0_TEXT_PHY, (dword*)&__TASK0_BSS_VMA, (dword*)&__TASK0_BSS_PHY, (dword*)&__TASK0_DATA_VMA, (dword*)&__TASK0_DATA_PHY, (dword*)&__TASK0_STACK_START_PHY, (dword*)&__TASK0_KERNEL_STACK_START_PHY, USE_MONITOR | DIGITS_TABLE_READ_WRITE, NULL, NULL);
    task1.tss->CR3 = GetCR3(PAG_PCD_YES, PAG_PWT_YES);
    SetTaskDirectoryPages((dword*)task1.tss->CR3, (dword*)&__TASK1_TEXT_VMA, (dword*)&__TASK1_TEXT_PHY, (dword*)&__TASK1_BSS_VMA, (dword*)&__TASK1_BSS_PHY, (dword*)&__TASK1_DATA_VMA, (dword*)&__TASK1_DATA_PHY, (dword*)&__TASK1_STACK_START_PHY, (dword*)&__TASK1_KERNEL_STACK_START_PHY, USE_MONITOR | USE_DINAMIC_PAGING | DIGITS_TABLE_READ, (dword*)&__TASK1_SIMD_VMA, (dword*)&__TASK1_SIMD_PHY);
    task2.tss->CR3 = GetCR3(PAG_PCD_YES, PAG_PWT_YES);
    SetTaskDirectoryPages((dword*)task2.tss->CR3, (dword*)&__TASK2_TEXT_VMA, (dword*)&__TASK2_TEXT_PHY, (dword*)&__TASK2_BSS_VMA, (dword*)&__TASK2_BSS_PHY, (dword*)&__TASK2_DATA_VMA, (dword*)&__TASK2_DATA_PHY, (dword*)&__TASK2_STACK_START_PHY, (dword*)&__TASK2_KERNEL_STACK_START_PHY, USE_MONITOR | DIGITS_TABLE_READ, (dword*)&__TASK2_SIMD_VMA, (dword*)&__TASK2_SIMD_PHY);
    Idle.tss->CR3 = GetCR3(PAG_PCD_YES, PAG_PWT_YES);
    SetTaskDirectoryPages((dword*)Idle.tss->CR3, (dword*)&__TASK3_TEXT_VMA, (dword*)&__TASK3_TEXT_PHY, (dword*)&__TASK3_BSS_VMA, (dword*)&__TASK3_BSS_PHY, (dword*)&__TASK3_DATA_VMA, (dword*)&__TASK3_DATA_PHY, (dword*)&__TASK3_STACK_START_PHY, (dword*)&__TASK3_KERNEL_STACK_START_PHY, BASIC_TASK, NULL, NULL);

    // agrego a la lista del scheduler las 3 tareas a ejecutar (la idle no va en la lista)
    scheduler.tasksAmount = 0;
    AddTaskToScheduler(task0);
    AddTaskToScheduler(task1);
    AddTaskToScheduler(task2);

    return;
}

__attribute__((section(".kernel32"))) static void AddTaskToScheduler(Task task)
{
    scheduler.tasks[scheduler.tasksAmount] = task;
    scheduler.tasksAmount++;
    return;
}

__attribute__((section(".kernel32"))) Task* GetTasksList()
{
    return scheduler.tasks;
}

__attribute__((section(".kernel32"))) dword GetTasksAmount()
{
    return scheduler.tasksAmount;
}

__attribute__((section(".kernel32"))) void InitTSS(TaskTSS* start, void* task, dword* simdSpace)
{
    start->EBX = 0;
    start->ECX = 0;
    start->EDX = 0;
    start->EAX = 0;
    start->ESI = 0;
    start->EDI = 0;
    start->EBP = 0;
    start->ESP3 = (dword)&__TASK_STACK_END_VMA;
    start->GS = (dword) &__DS_SEL_32_3_INDEX;
    start->FS = (dword) &__DS_SEL_32_3_INDEX;
    start->SS3 = (dword) &__DS_SEL_32_3_INDEX;
    start->ES = (dword) &__DS_SEL_32_3_INDEX;
    start->DS = (dword) &__DS_SEL_32_3_INDEX;
    start->CS = (dword) &__CS_SEL_32_3_INDEX;
    start->ESP0 = (dword) &__TASK_KERNEL_STACK_END_VMA;
    start->SS0 = (dword) &__DS_SEL_32_INDEX;
    start->EIP = (dword) task;
    start->EFLAGS = (dword) 0x202;
    start->TaskCounter = 0;
    start->Loaded = 0;
    start->SimdSpace = simdSpace;
}
