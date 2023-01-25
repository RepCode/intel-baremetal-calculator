#ifndef PAGING_HANDLER_H
#define PAGING_HANLDER_H

#define PAG_PWT_NO  0
#define PAG_PWT_YES 1
#define PAG_PCD_NO  0
#define PAG_PCD_YES 1

#define P_YES       1
#define P_NO        0
#define RW_YES      1
#define RW_NO       0 // Read-only
#define US_ALL      1
#define US_SUP      0
#define WTHR_YES    1
#define WTHR_NO     0
#define PCD_YES     0
#define PCD_NO      1
#define ACC_NO      0
#define ACC_YES     1
#define SIZE_4K     0
#define SIZE_4M     1

#define G_YES       1
#define G_NO        0
#define D_NO        0

// task configs
#define BASIC_TASK 0b0
#define USE_MONITOR 0b01
#define IS_USE_MONITOR(x) (x & 0b1)
#define USE_DINAMIC_PAGING 0b10
#define IS_USE_DINAMIC_PAGING(x) ((x>>1) & 0b1)
#define DIGITS_TABLE_READ 0b100
#define IS_DIGITS_TABLE_READ(x) ((x>>2) & 0b1)
#define DIGITS_TABLE_READ_WRITE 0b1000
#define IS_DIGITS_TABLE_READ_WRITE(x) ((x>>3) & 0b1)


typedef struct SimdSectionStruct{
    dword* SimdSectionVma;
    dword* SimdSectionPhy;
} SimdSection;

dword GetCR3(byte pcd, byte pwt);
dword GetBootPageDirectory();
void SetBootPages();
void SetDirPageTableEntry(dword *dptBase, word entry, byte ps, byte a, byte pcd, byte pwt, byte us, byte rw, byte p);
void SetPageTableEntries(dword directoryIndex, word entry, dword ptpBase, byte g, byte d, byte a, byte pcd, byte pwt, byte us, byte rw, byte p);
void AddNewPage();
void SetTaskDirectoryPages(dword* dpt, dword* taskTextVMA, dword* taskTextPHY, dword* taskBssVMA, dword* taskBssPHY, dword* taskDataVMA, dword* taskDataPHY, dword* taskStackPHY, dword* taskKernelStackPHY, byte config, dword* taskSimdVMA, dword* taskSimdPHY);
void SetEntryAndPageIt(dword *dptBase, word entry, byte ps, byte a, byte pcd, byte pwt, byte us, byte rw, byte p);
byte AddSimdSection(dword* sectionStart, dword* sectionStartPhy);

extern void* __PAGETABLES_VMA;
extern void* __IRQ_HANDLERS_VMA;
extern void* __FUNCTIONS_VMA;
extern void* MONITOR_RAM_START_VMA;
extern void* MONITOR_RAM_START_PHY;
extern void* __SYS_TABLES_START;
extern void* __KERNEL_32_VMA;
extern void* __KERNEL_32_PHY;
extern void* __INITIALISED_VMA;
extern void* __INITIALISED_PHY;
extern void* __UNINITIALISED_VMA;
extern void* __UNINITIALISED_PHY;
extern void* __DIGITS_TABLE_VMA;
extern void* __DIGITS_TABLE_PHY;
extern void* __SET_DESCTIPTOR_TABLES_VMA;

extern void* __TASK2_TEXT_VMA;
extern void* __TASK2_TEXT_PHY;
extern void* __TASK2_BSS_VMA;
extern void* __TASK2_BSS_PHY;
extern void* __TASK2_DATA_VMA;
extern void* __TASK2_DATA_PHY;

extern void* __TASK0_TEXT_VMA;
extern void* __TASK0_TEXT_PHY;
extern void* __TASK0_BSS_VMA;
extern void* __TASK0_BSS_PHY;
extern void* __TASK0_DATA_VMA;
extern void* __TASK0_DATA_PHY;

extern void* __STACK_START_32;
extern void* __PAGETABLES_VMA;
extern void* __INIT_32_VMA;
extern void* __TASK_STACK_START_VMA;
extern void* __TASK1_STACK_START_PHY;
extern void* __TASK2_STACK_START_PHY;
extern void* __TASK0_STACK_START_PHY;
extern void* __TASK_KERNEL_STACK_START_VMA;
extern void* __INIT_BSS_VMA;
extern void* __BASIC_TSS;

extern void* __SYS_CALLS_API_VMA;
extern void* __SYS_CALLS_API_PHY;

#endif