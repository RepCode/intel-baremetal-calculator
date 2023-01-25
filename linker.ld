/*
    Linker script del sistema. La ROM esta compactada una seccion a coninuacion de la otra para ocupar lo minimo posible
    y mantenerse flexible a medida que una seccion crece o se reduce de tamano
*/

ENTRY (reset)

    __EH_FRAME = 0x00060000; /*Exception handling area created by gcc*/
    __STACK_START_16 = 0x9000;
    __STACK_END_16 = 0x9FFF;
    __STACK_START_32 = 0x1FF08000;
    __STACK_END_32 = 0x1FF08FFF;
    __STACK_SIZE_32 = ((__STACK_END_32 - __STACK_START_32) / 4);

    /* Monitor */
    MONITOR_RAM_START_VMA = 0x00010000;
    MONITOR_RAM_START_PHY = 0x000B8000;

    /* Reset: first executed section */
    __RESET_LMA = 0xFFFFFFF0;
    __RESET_VMA = 0xFFFFFFF0;

    /* base init16: after reset (still in real mode), the processor jumps here where base initialization occurs:
    16bits stack, cache disable, A20 gate init, then copies the rest of the inicialization to ram */
    __INIT_16_LMA = 0xFFFF0000;
    __INIT_16_VMA = 0xFFFF0000;

    /* Basic TSS */
    __BASIC_TSS = 0x04300000;

    /* Sys Calls Api */
    __SYS_CALLS_API_VMA = 0x01400000;
    __SYS_CALLS_API_PHY = 0x00400000;

    /* TASK Shared VMA */
    __TASK_STACK_START_VMA = 0x00713000;
    __TASK_STACK_END_VMA = 0x00713FFF;
    __TASK_STACK_SIZE_32 = ((__TASK_STACK_END_VMA - __TASK_STACK_START_VMA) / 4);
    __TASK_KERNEL_STACK_START_VMA = 0x00714000;
    __TASK_KERNEL_STACK_END_VMA = 0x00714FFF;
    __TASK_KERNEL_STACK_SIZE_32 = ((__TASK_KERNEL_STACK_END_VMA - __TASK_KERNEL_STACK_START_VMA) / 4);

    /* TASK 1 */
    __TASK1_TEXT_VMA = 0x01300000;
    __TASK1_TEXT_PHY = 0x00300000;

    __TASK1_BSS_VMA = 0x01301000;
    __TASK1_BSS_PHY = 0x00301000;

    __TASK1_DATA_VMA = 0x01302000;
    __TASK1_DATA_PHY = 0x00302000;

    __TASK1_SIMD_VMA = 0x01303000;
    __TASK1_SIMD_PHY = 0x00303000;

    __TASK1_STACK_START_PHY = 0x1FFFF000;
    __TASK1_KERNEL_STACK_START_PHY = 0x1FF07000;

    /* TASK 2 */
    __TASK2_TEXT_VMA = 0x01310000;
    __TASK2_TEXT_PHY = 0x00310000;

    __TASK2_BSS_VMA = 0x01311000;
    __TASK2_BSS_PHY = 0x00311000;

    __TASK2_DATA_VMA = 0x01312000;
    __TASK2_DATA_PHY = 0x00312000;

    __TASK2_SIMD_VMA = 0x01313000;
    __TASK2_SIMD_PHY = 0x00313000;

    __TASK2_STACK_START_PHY = 0x1FFFE000;
    __TASK2_KERNEL_STACK_START_PHY = 0x1FF06000;

    /* TASK 0 */
    __TASK0_TEXT_VMA = 0x01320000;
    __TASK0_TEXT_PHY = 0x00320000;

    __TASK0_BSS_VMA = 0x01321000;
    __TASK0_BSS_PHY = 0x00321000;

    __TASK0_DATA_VMA = 0x01322000;
    __TASK0_DATA_PHY = 0x00322000;

    __TASK0_STACK_START_PHY = 0x1FFFD000;
    __TASK0_KERNEL_STACK_START_PHY = 0x1FF04000;

    /* TASK 3 */
    __TASK3_TEXT_VMA = 0x01330000;
    __TASK3_TEXT_PHY = 0x00330000;

    __TASK3_BSS_VMA = 0x01331000;
    __TASK3_BSS_PHY = 0x00331000;

    __TASK3_DATA_VMA = 0x01332000;
    __TASK3_DATA_PHY = 0x00332000;

    __TASK3_STACK_START_PHY = 0x1FFFC000;
    __TASK3_KERNEL_STACK_START_PHY = 0x1FF05000;

    /* to keep position of gtd segments */
    __SET_DESCTIPTOR_TABLES_VMA = 0x00299000;

    /* Digits table */
    __DIGITS_TABLE_VMA = 0x01210000;
    __DIGITS_TABLE_PHY = 0x00210000;

    /* uninitialised system data */
    __UNINITIALISED_VMA = 0x01203000;
    __UNINITIALISED_PHY = 0x00203000;

    /* initialised system data */
    __INITIALISED_VMA = 0x01202000;
    __INITIALISED_PHY = 0x00202000;

    /*  kernel */
    __KERNEL_32_VMA = 0x01200000;
    __KERNEL_32_PHY = 0x00200000;

    /* PAGES */
    __PAGETABLES_VMA = 0x00110000;

    /* SYSTABLES: where init32 will create the gdt and idts */
    __SYS_TABLES_START = 0x00100000;
    __CS_SEL_32_INDEX = 0x0008;
    __DS_SEL_32_INDEX = 0x00010;
    __CS_SEL_32_3_INDEX = 0x0018;
    __DS_SEL_32_3_INDEX = 0x00020;
    __TSS_SELL_INDEX = 0x00028;

    /* functions: all routines of the program */
    __FUNCTIONS_VMA = 0x00004000;

    /* BSS variables used on init before and after paging is on (need identity Mapping) */
    __INIT_BSS_VMA = 0x0000F000;

    /* IRQ handlers section */
    __IRQ_HANDLERS_VMA = 0x00000000;

MEMORY
{
    ram (!x) : ORIGIN = 0x00000000, LENGTH = 0xFFFF0000
    rom (rx) : ORIGIN = 0xFFFF0000, LENGTH = 0xFFFF
}

SECTIONS
{

    .digits_table __DIGITS_TABLE_VMA (NOLOAD) :
        { *(.digits_table); } > ram
    _digits_table_size = SIZEOF(.digits_table);

    .uninitialised_data __UNINITIALISED_VMA (NOLOAD) :
        { *(.uninitialised_data*); } > ram
    __uninitialised_data_size = SIZEOF(.uninitialised_data);

    .task1_bss __TASK1_BSS_VMA (NOLOAD) :
        { *(.task1_bss*); } > ram
    __task1_bss_size = SIZEOF(.task1_bss);

    .task2_bss __TASK2_BSS_VMA (NOLOAD) :
        { *(.task2_bss*); } > ram
    __task2_bss_size = SIZEOF(.task2_bss);

    .task0_bss __TASK0_BSS_VMA (NOLOAD) :
        { *(.task0_bss*); } > ram
    __task0_bss_size = SIZEOF(.task0_bss);

    .task3_bss __TASK3_BSS_VMA (NOLOAD) :
        { *(.task3_bss*); } > ram
    __task3_bss_size = SIZEOF(.task3_bss);

    .init_bss __INIT_BSS_VMA (NOLOAD) :
        { *(.init_bss*); } > ram
    __init_bss_size = SIZEOF(.init_bss);

    .task1_simd __TASK1_SIMD_VMA (NOLOAD) :
        { *(.task1_simd*); } > ram
    __task1_simd_size = SIZEOF(.task1_simd);

    .task2_simd __TASK2_SIMD_VMA (NOLOAD) :
        { *(.task2_simd*); } > ram
    __task2_simd_size = SIZEOF(.task2_simd);

    .codigo_init16 __INIT_16_VMA :
        AT( __INIT_16_LMA )
        { *(.init16); } > rom
    __init16_size = SIZEOF(.codigo_init16);

    __IRQ_HANDLERS_LMA = LOADADDR(.codigo_init16) + __init16_size;
    .irq_handlers __IRQ_HANDLERS_VMA :
        AT( __IRQ_HANDLERS_LMA )
        { *(.irq_handlers); } > ram
    __irq_handlers_size = SIZEOF(.irq_handlers);

    __FUNCTIONS_LMA = LOADADDR(.irq_handlers) + __irq_handlers_size;
    .functions __FUNCTIONS_VMA :
        AT( __FUNCTIONS_LMA )
        { *(.functions); } > ram
    __functions_size = SIZEOF(.functions);

    __KERNEL_32_LMA = LOADADDR(.functions) + __functions_size;
    .codigo_kernel32 __KERNEL_32_VMA :
        AT ( __KERNEL_32_LMA )
        { *(.kernel32*); } > ram
    __codigo_kernel32_size = SIZEOF(.codigo_kernel32);

    __SET_DESCTIPTOR_TABLES_LMA = LOADADDR(.codigo_kernel32) + __codigo_kernel32_size;
    .codigo_set_descriptor_tables __SET_DESCTIPTOR_TABLES_VMA :
        AT ( __SET_DESCTIPTOR_TABLES_LMA )
        { *(.set_dt); } > ram
    __codigo_set_descriptor_tables_size = SIZEOF(.codigo_set_descriptor_tables);

    __INITIALISED_LMA = LOADADDR(.codigo_set_descriptor_tables) + __codigo_set_descriptor_tables_size;
    .initialised_data __INITIALISED_VMA :
        AT ( __INITIALISED_LMA )
        { *(.initialised_data*); } > ram
    __initialised_data_size = SIZEOF(.initialised_data);

    __SYS_CALLS_API_LMA = LOADADDR(.initialised_data) + __initialised_data_size;
    .sys_calls_api __SYS_CALLS_API_VMA :
        AT( __SYS_CALLS_API_LMA )
        { *(.sys_calls_api*); } > ram
    __sys_calls_api_size = SIZEOF(.sys_calls_api);

    __TASK1_TEXT_LMA = LOADADDR(.sys_calls_api) + __sys_calls_api_size;
    .task1_text __TASK1_TEXT_VMA :
        AT( __TASK1_TEXT_LMA )
        { *(.task1_text*); } > ram
    __task1_text_size = SIZEOF(.task1_text);

    __TASK1_DATA_LMA = LOADADDR(.task1_text) + __task1_text_size;
    .task1_data __TASK1_DATA_VMA :
        AT( __TASK1_DATA_LMA )
        { *(.task1_data*); } > ram
    __task1_data_size = SIZEOF(.task1_data);

    __TASK2_TEXT_LMA = LOADADDR(.task1_data) + __task1_data_size;
    .task2_text __TASK2_TEXT_VMA :
        AT( __TASK2_TEXT_LMA )
        { *(.task2_text*); } > ram
    __task2_text_size = SIZEOF(.task2_text);

    __TASK0_TEXT_LMA = LOADADDR(.task2_text) + __task2_text_size;
    .task0_text __TASK0_TEXT_VMA :
        AT( __TASK0_TEXT_LMA )
        { *(.task0_text*); } > ram
    __task0_text_size = SIZEOF(.task0_text);

    __TASK3_TEXT_LMA = LOADADDR(.task0_text) + __task0_text_size;
    .task3_text __TASK3_TEXT_VMA :
        AT( __TASK3_TEXT_LMA )
        { *(.task3_text*); } > ram
    __task3_text_size = SIZEOF(.task3_text);

    __TASK2_DATA_LMA = LOADADDR(.task3_text) + __task3_text_size;
    .task2_data __TASK2_DATA_VMA :
        AT( __TASK2_DATA_LMA )
        { *(.task2_data*); } > ram
    __task2_data_size = SIZEOF(.task2_data);

    __TASK0_DATA_LMA = LOADADDR(.task2_data) + __task2_data_size;
    .task0_data __TASK0_DATA_VMA :
        AT( __TASK0_DATA_LMA )
        { *(.task0_data*); } > ram
    __task0_data_size = SIZEOF(.task0_data);

    __TASK3_DATA_LMA = LOADADDR(.task0_data) + __task0_data_size;
    .task3_data __TASK3_DATA_VMA :
        AT( __TASK3_DATA_LMA )
        { *(.task3_data*); } > ram
    __task3_data_size = SIZEOF(.task3_data);

    __INIT_32_LMA = LOADADDR(.task3_data) + __task3_data_size;
    __INIT_32_VMA = __INIT_32_LMA;
    .codigo_init32 __INIT_32_VMA :
        AT(__INIT_32_LMA)
        { *(.start32*); } > rom
    __codigo_init32_size = SIZEOF(.codigo_init32);

    __FUNCTIONS_ROM_LMA = LOADADDR(.codigo_init32) + __codigo_init32_size;
    __FUNCTIONS_ROM_VMA = __FUNCTIONS_ROM_LMA;
    .functions_rom __FUNCTIONS_ROM_VMA :
        AT(__FUNCTIONS_ROM_LMA)
        { *(.functions_rom*); } > rom
    __functions_rom_size = SIZEOF(.functions_rom);

    __INIT_GDT_LMA = LOADADDR(.functions_rom) + __functions_rom_size;
    __INIT_GDT_VMA = __INIT_GDT_LMA;
    .init_gdt __INIT_GDT_VMA :
        AT(__INIT_GDT_LMA)
        { *(.init_gdt*); } > rom
    __data_size = SIZEOF(.init_gdt);

    __BSS_LMA = LOADADDR(.init_gdt) + __data_size;
    __BSS_VMA = __BSS_LMA;
    .bss __BSS_VMA :
        AT(__BSS_LMA)
        { *(.bss*); } > rom

    .codigo_reset __RESET_VMA :
        AT(__RESET_LMA)
        { *(.resetVector); } > rom


    .fill_rom :
        { FILL(0x90);
            . = ORIGIN(rom) + LENGTH(rom)-1;
            BYTE(0x90);
        } > rom

    .end_rom 0xFFFFFFFF :
        AT(0xFFFFFFFF)
        {
            BYTE(0x90);
        }
}
