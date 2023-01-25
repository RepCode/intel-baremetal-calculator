#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "commonHelpers.h"

#define TASKS_AMOUNT 5

typedef struct __attribute__((packed)) TaskTSS_Struct
{
    dword   EBX; // 0
    dword   ECX; // 4
    dword   EDX; // 8
    dword   EAX; // 12
    dword   ESI; // 16
    dword   EDI; // 20
    dword   EBP; // 24
    dword   ESP3; // 28
    dword   GS; // 32
    dword   FS; // 36
    dword   SS3; // 40
    dword   ES; // 44
    dword   DS; // 48
    dword   CR3; // 52
    dword   CS; // 56
    dword   ESP0;// 60
    dword   SS0; // 64
    dword   EIP; // 68
    dword   EFLAGS; // 72
    dword   TaskCounter; // 76
    byte   Loaded; // 80
    dword*  SimdSpace;
} TaskTSS;

typedef enum TaskStatusEnum
{
    WAITING,
    RUNNING,
    FINISHED
}TaskStatus;

typedef struct TaskStruct
{
    dword period;
    dword count;
    void (*Run)(void);
    TaskStatus status;
    TaskTSS* tss;
    byte ID[4];
} Task;

typedef struct SchedulerStruct
{
    Task    tasks[TASKS_AMOUNT];
    dword     tasksAmount;
} Scheduler;

Task* GetTasksList();
dword GetTasksAmount();
void InitTSS(TaskTSS* start, void* task, dword* simdSpace);
void SetScheduler();
void FinishTask();

extern  void* __STACK_END_32;
extern  void* __TASK_STACK_END_VMA;
extern  void* __TASK_KERNEL_STACK_END_VMA;
extern  void* __CS_SEL_32_INDEX;
extern  void* __DS_SEL_32_INDEX;
extern  void* __CS_SEL_32_3_INDEX;
extern  void* __DS_SEL_32_3_INDEX;

extern void* __TASK0_TEXT_VMA;
extern void* __TASK0_TEXT_PHY;
extern void* __TASK0_BSS_VMA;
extern void* __TASK0_BSS_PHY;
extern void* __TASK0_DATA_VMA;
extern void* __TASK0_DATA_PHY;
extern void* __TASK0_STACK_START_PHY;
extern void* __TASK0_KERNEL_STACK_START_PHY;

extern void* __TASK1_TEXT_VMA;
extern void* __TASK1_TEXT_PHY;
extern void* __TASK1_BSS_VMA;
extern void* __TASK1_BSS_PHY;
extern void* __TASK1_DATA_VMA;
extern void* __TASK1_DATA_PHY;
extern void* __TASK1_STACK_START_PHY;
extern void* __TASK1_KERNEL_STACK_START_PHY;
extern void* __TASK1_SIMD_VMA;
extern void* __TASK1_SIMD_PHY;

extern void* __TASK2_TEXT_VMA;
extern void* __TASK2_TEXT_PHY;
extern void* __TASK2_BSS_VMA;
extern void* __TASK2_BSS_PHY;
extern void* __TASK2_DATA_VMA;
extern void* __TASK2_DATA_PHY;
extern void* __TASK2_STACK_START_PHY;
extern void* __TASK2_KERNEL_STACK_START_PHY;
extern void* __TASK2_SIMD_VMA;
extern void* __TASK2_SIMD_PHY;

extern void* __TASK3_TEXT_VMA;
extern void* __TASK3_TEXT_PHY;
extern void* __TASK3_BSS_VMA;
extern void* __TASK3_BSS_PHY;
extern void* __TASK3_DATA_VMA;
extern void* __TASK3_DATA_PHY;
extern void* __TASK3_STACK_START_PHY;
extern void* __TASK3_KERNEL_STACK_START_PHY;

#endif
