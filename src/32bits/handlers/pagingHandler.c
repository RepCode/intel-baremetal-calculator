/*
    Archivo encargado de toda la paginacion del sistema. Esta desarrollado para ocupar la menor cantidad de memoria posible. Para esto las tablas se van agregando
    como una pila, no se dejan huecos en el medio para uso futuro. Esto se logra a traves del index NextEntryIndex que todas las funciones usan y es donde agregan una
    nueva pagina cuando la utilizan. Tambien es importante que una vez que se entra a modo paginacion para agregar una nueva pagina antes hay que paginar su entrada en el directorio
    de esta forma se logra una paginacion dinamica sin la necesidad de tener espacio inutilizado previo a cuando se va a utilizar ni ejecutar codigo para escribir entries que potencialmente
    no se van a utilizar.
*/

#include "../../../inc/commonHelpers.h"
#include "../../../inc/pagingHandler.h"
#include "../../../inc/scheduler.h"

#define PAGE_TABLES_START ((dword) &__PAGETABLES_VMA)
#define ENTRY_POSITION(x)   (((dword)(x) >> 12) & 0x3FF)
#define PAGE_TABLE(x)   (((dword)(x) >> 22) & 0x3FF)

#define PAGE_TABLES_INDEX 1

__attribute__((section(".init_bss"))) static dword NextEntryIndex = 0;
__attribute__((section(".init_bss"))) static SimdSection SimdRegisters[TASKS_AMOUNT] = { { .SimdSectionVma=NULL, .SimdSectionPhy=NULL } };
__attribute__((section(".uninitialised_data"))) static dword ExceptionPagedCount = 0;

// esta funcion esta pensando en una forma escalable te tener varias tareas que usan SIMD en ejecucion a la vez. Durante su inicializacion
// las tareas informan que van a utilizar simd, entonces la seccion donde se guardan sus registros se agrega a esta array
// para asi poder paginar en todas las tareas que tienen SIMD el area donde se guardan los registros de las otras tareas (y poder hacer el switch).
__attribute__((section(".functions")))inline byte AddSimdSection(dword* sectionStartVma, dword* sectionStartPhy)
{
    int i;
    for(i=0; i<TASKS_AMOUNT; i++)
    {
        if(SimdRegisters[i].SimdSectionVma == NULL && SimdRegisters[i].SimdSectionPhy == NULL){
            SimdRegisters[i].SimdSectionVma = sectionStartVma;
            SimdRegisters[i].SimdSectionPhy = sectionStartPhy;
            return 0;
        }
    }
    return -1;
}

// Esta funcion crea un nuevo directorio en el proximo index de la tabla de paginas.
__attribute__((section(".functions"))) dword GetCR3(byte pcd, byte pwt)
{
    dword _cr3 = 0;
    
    SetPageTableEntries(PAGE_TABLES_INDEX, ENTRY_POSITION(PAGE_TABLES_START + 0x1000 * NextEntryIndex), (dword) (PAGE_TABLES_START + 0x1000 * NextEntryIndex), G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);

    _cr3 |= ((PAGE_TABLES_START + 0x1000*NextEntryIndex) & 0xFFFFF000);
    _cr3 |= (pcd << 4);
    _cr3 |= (pcd << 3);
    NextEntryIndex++;
    
    return _cr3;
}

// esta funcion abstrae de la inicializacion la configuracion de la paginacion. Le da el CR3 para que lo carge directamente.
__attribute__((section(".functions")))inline dword GetBootPageDirectory()
{
    return GetCR3(PAG_PCD_YES, PAG_PWT_YES);
}

// Pagina todas las direcciones de memorias que va a necesitar el kernel para realizar las tareas restantes previas a la ejecucion de las tareas.
// Estas son. Inicializar el scheduler, paginar los contextos de las tareas, imprimir en pantalla y activar las interrupciones.
__attribute__((section(".functions"))) inline void SetBootPages()
{
    dword* dpt = (dword*)PAGE_TABLES_START;
    int currentPageIndex = NextEntryIndex;

    SetEntryAndPageIt(dpt, 0x0, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__IRQ_HANDLERS_VMA),      (dword) &__IRQ_HANDLERS_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INIT_BSS_VMA),          (dword) &__INIT_BSS_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__SYS_TABLES_START),      (dword) &__SYS_TABLES_START, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__FUNCTIONS_VMA),         (dword) &__FUNCTIONS_VMA, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__PAGETABLES_VMA),        (dword) &__PAGETABLES_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&MONITOR_RAM_START_VMA),   (dword) &MONITOR_RAM_START_PHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__SET_DESCTIPTOR_TABLES_VMA), (dword) &__SET_DESCTIPTOR_TABLES_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x4, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__KERNEL_32_VMA),         (dword) &__KERNEL_32_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INITIALISED_VMA),       (dword) &__INITIALISED_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__UNINITIALISED_VMA),     (dword) &__UNINITIALISED_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x10, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__BASIC_TSS),       (dword) &__BASIC_TSS, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x7F, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__STACK_START_32),       (dword) &__STACK_START_32, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x3FF, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INIT_32_VMA),       (dword) &__INIT_32_VMA, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INIT_32_VMA)+1,       (dword) (&__INIT_32_VMA)+0x1000, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
}

// simplifica la inicializacion de las paginas del contexto de una tarea. En config se le configura que cosas en particular tiene la tarea. Por ejemplo si va a imprimir en el monitor o no
// y se pagina acorde a lo que se va a utilizar y con los permisos correctos.
__attribute__((section(".functions"))) void SetTaskDirectoryPages(dword* dpt, dword* taskTextVMA, dword* taskTextPHY, dword* taskBssVMA, dword* taskBssPHY, dword* taskDataVMA, dword* taskDataPHY, dword* taskStackPHY, dword* taskKernelStackPHY, byte config, dword* taskSimdVMA, dword* taskSimdPHY)
{
    int currentPageIndex = NextEntryIndex;
    int i;
    dpt = (dword*)((dword)dpt & 0xFFFFF000);
    // Cuando ejecuto esta funcion ya estoy con modo paginacion activado. Por lo que cada entry nuevo debe ser agregado antes al directorio o habra un excepcion
    // esta funcion se encarga de agregar el entry previo a escribir en la pagina.
    SetEntryAndPageIt(dpt, 0x0, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__IRQ_HANDLERS_VMA),      (dword) &__IRQ_HANDLERS_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INIT_BSS_VMA),          (dword) &__INIT_BSS_VMA, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__SYS_TABLES_START),      (dword) &__SYS_TABLES_START, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
        #ifndef DEBUG
        if(IS_USE_MONITOR(config)) // si estoy en debug todos los contextos necesitan monitor
        #endif
        SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&MONITOR_RAM_START_VMA),   (dword) &MONITOR_RAM_START_PHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    
    // Si la tarea tiene permitido expandir su espacio de memoria.
    if(IS_USE_DINAMIC_PAGING(config))
    {
        SetPageTableEntries(currentPageIndex, ENTRY_POSITION(PAGE_TABLES_START + 0x1000 * currentPageIndex-1), (dword) (PAGE_TABLES_START + 0x1000 * currentPageIndex-1), G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
        SetPageTableEntries(currentPageIndex, ENTRY_POSITION(PAGE_TABLES_START + 0x1000 * currentPageIndex), (dword) (PAGE_TABLES_START + 0x1000 * currentPageIndex), G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
        SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__FUNCTIONS_VMA),         (dword) &__FUNCTIONS_VMA, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    }

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x1, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__TASK_STACK_START_VMA), (dword)taskStackPHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__TASK_KERNEL_STACK_START_VMA), (dword)taskKernelStackPHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x4, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__KERNEL_32_VMA),         (dword) &__KERNEL_32_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__INITIALISED_VMA),       (dword) &__INITIALISED_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__UNINITIALISED_VMA),     (dword) &__UNINITIALISED_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(taskTextVMA), (dword)taskTextPHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(taskBssVMA), (dword)taskBssPHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(taskDataVMA), (dword)taskDataPHY, G_NO, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_YES, P_YES);

    // Si la tarea usa la tabla de digitos
    if(IS_DIGITS_TABLE_READ(config) || IS_DIGITS_TABLE_READ_WRITE(config))
    {
        SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__DIGITS_TABLE_VMA),      (dword) &__DIGITS_TABLE_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, IS_DIGITS_TABLE_READ_WRITE(config) ? RW_YES : RW_NO, P_YES); // seteo acorde si tiene permisos de R/W o R solo.
    }
    
    // Si la tarea usa SIMD
    if(taskSimdVMA != NULL)
    {
        for(i=0; i<TASKS_AMOUNT; i++)
        {
            if(SimdRegisters[i].SimdSectionVma == NULL || SimdRegisters[i].SimdSectionPhy == NULL){
                break;
            }
            SetPageTableEntries(currentPageIndex, ENTRY_POSITION(SimdRegisters[i].SimdSectionVma), (dword)SimdRegisters[i].SimdSectionPhy, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
        }
    }
    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x5, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_NO, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__SYS_CALLS_API_VMA), (dword) &__SYS_CALLS_API_PHY, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_ALL, RW_NO, P_YES);

    currentPageIndex = NextEntryIndex;
    SetEntryAndPageIt(dpt, 0x10, SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetPageTableEntries(currentPageIndex, ENTRY_POSITION(&__BASIC_TSS), (dword) &__BASIC_TSS, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
}

// Antes de agregar cada entry nuevo lo deja paginado para asegurar que se accedible una vez activada paginación.
// Necesario para poder configurar el CR3 de las tareas con paginación activada.
__attribute__((section(".functions"))) void SetEntryAndPageIt(dword *dptBase, word entry, byte ps, byte a, byte pcd, byte pwt, byte us, byte rw, byte p)
{
    SetPageTableEntries(PAGE_TABLES_INDEX, ENTRY_POSITION(PAGE_TABLES_START + 0x1000 * NextEntryIndex), (dword) (PAGE_TABLES_START + 0x1000 * NextEntryIndex), G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_YES, P_YES);
    SetDirPageTableEntry(dptBase, entry, ps, a, pcd, pwt, us, rw, p);
}

__attribute__((section(".functions"))) void SetDirPageTableEntry(dword *dptBase, word entry, byte ps, byte a, byte pcd, byte pwt, byte us, byte rw, byte p)
{
    dword dpte = 0;

    dpte |= ( (PAGE_TABLES_START+0x1000*NextEntryIndex) & 0xFFFFF000 );
    dpte |= ( ps << 7 );
    dpte |= ( a << 5 );
    dpte |= ( pcd << 4 );
    dpte |= ( pwt << 3 );
    dpte |= ( us << 2 );
    dpte |= ( rw << 1 );
    dpte |= p;
    *(dptBase+entry) = dpte;
    
    NextEntryIndex++;
}

__attribute__((section(".functions"))) void SetPageTableEntries(dword directoryIndex, word entry, dword ptpBase, byte g, byte d, byte a, byte pcd, byte pwt, byte us, byte rw, byte p)
{
    dword dpte = 0;

    dpte |= (ptpBase) & 0xFFFFF000;
    dpte |= ( g << 8 );
    dpte |= ( d << 5 );
    dpte |= ( pcd << 4 );
    dpte |= ( pwt << 3 );
    dpte |= ( us << 2 );
    dpte |= ( rw << 1 );
    dpte |= p;
    *(((dword *)(PAGE_TABLES_START+0x1000*directoryIndex))+entry) = dpte;
}

// funcion utilizada por la rutina de interrupcion de la Page Fault. Obtiene el directorio actual a partir de cr3 y de ahi
// agrega una pagina si es que hace falta y si no hace falta solo agrega el entry a la pagina correcta.
__attribute__((section(".functions"))) void AddNewPage(dword vmaAddress)
{
    dword _cr3, directoryIndex, entry;
    asm("mov %%cr3, %0" : "=r" (_cr3));
    _cr3 &= 0xFFFFF000;

    entry = *((dword*)_cr3 + PAGE_TABLE(vmaAddress)); // obtengo el entry en el directory
    if(!(entry & 0x01)) // si no esta presente
    {
        entry = PAGE_TABLES_START + 0x1000 * NextEntryIndex;
        directoryIndex = (_cr3 >> 12) - (PAGE_TABLES_START >> 12) + 1;
        SetPageTableEntries(directoryIndex, ENTRY_POSITION(entry), (dword) (entry), G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
        SetDirPageTableEntry((dword*)_cr3, PAGE_TABLE(vmaAddress), SIZE_4K, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
    }
    directoryIndex = (entry >> 12) - (PAGE_TABLES_START >> 12);
    SetPageTableEntries(directoryIndex, ENTRY_POSITION(vmaAddress), 0x08000000 + 0x1000 * ExceptionPagedCount++, G_YES, D_NO, ACC_NO, PCD_NO, WTHR_NO, US_SUP, RW_NO, P_YES);
}
