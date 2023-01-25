/*
    Este archivo centraliza toda la configuracion de la GDT e IDT. Van una debado de la otra y el archivo tiene indices globales para
    adaptarse correctamente, es decir que si agrego un nuevo descriptor a la GDT se desplaza todo para abajo(en compile time)
*/

#include "../../../inc/commonHelpers.h"

extern void __interrupHandler__0();
extern void __interrupHandlerend__0();
extern void __devide_zeroIRQ__();
extern void __general_protectionIRQ__();
extern void __invalid_opcodeIRQ__();
extern void __double_faultIRQ__();
extern void __keyboardIRQ__();
extern void __systickIRQ__();
extern void __pageFaultIRQ__();
extern void __deviceNotAvailable();
extern void __sys_call();
extern char* __SYS_TABLES_START;
extern char* __CS_SEL_32_INDEX;

#define default_handler_code_size (dword)(__interrupHandlerend__0 - __interrupHandler__0)

// estructura para ordenar los campos de la GDT
typedef struct descriptor_table_struct {
    // byte 0 and 1
    word limit_15_0;
    // byte 2 and 3
    word base_15_0;
    // byte 4
    byte base_23_16;
    // byte 5
    byte p;
    byte dpl;
    byte s;
    byte type;
    // byte 6
    byte g;
    byte d_b;
    byte l;
    byte avl;
    byte limit19_16;
    // byte 7
    byte base_31_24;
} descriptor_table;

void addGlobalDescriptorTable(descriptor_table newTable);
void addInterruptDescriptorTable(int index, int offset, int segmentSelector, char type, char dpl, char p, char ist);

// variables donde se cargaran base y tamano de la gdt e idt para hacer el load mas adelante
qword _gdtr __attribute__((section(".init_bss")));
qword _idtr __attribute__((section(".init_bss")));

static dword descriptor_tables_index __attribute__((section(".init_bss")));

// esta funcion abstrae toda la inicializacion de descriptores. Ya que se va a trabajar con un modelo flat y luego utilizar paginacion
// solamente se llama a esta funcion que se encarga de inicializar los descriptores necesarios para utilizar toda la RAM.
void __attribute__ ((section (".set_dt")))SetGlobalDescriptionTables()
{
    byte* dtPointer = (char *)&_gdtr;
    short *dtrLimit = (short*)dtPointer; // dtrLimit apunta a la base de donde guardo gdtr (que es donde se guarda limit)
    int *dtrBase = (int *) (dtPointer+2); // me corro 16bits a los 32bits de base de la gdt
    *dtrBase = (int)&__SYS_TABLES_START;

    // Descriptor nulo
    descriptor_table nullDT =
     { .base_31_24=0x00, .g=0b0, .d_b=0b0, .l=0b0, .avl=0b0, .limit19_16=0b0000,
     .p=0b0, .dpl=0b00, .s=0b0, .type=0b0000, .base_23_16=0x00, .base_15_0=0x0000, .limit_15_0=0x0000 };

    // Descriptor de codigo de nivel 0 (supervisor)
    descriptor_table CS_SEL0 =
     { .base_31_24=0x00, .g=0b1, .d_b=0b1, .l=0b0, .avl=0b0, .limit19_16=0b1111,
     .p=0b1, .dpl=0b00, .s=0b1, .type=0b1001, .base_23_16=0x00, .base_15_0=0x0000, .limit_15_0=0xFFFF };

    // Descriptor de datos de nivel 0 (supervisor)
    descriptor_table DS_SEL0 =
     { .base_31_24=0x00, .g=0b1, .d_b=0b1, .l=0b0, .avl=0b0, .limit19_16=0b1111,
     .p=0b1, .dpl=0b00, .s=0b1, .type=0b0010, .base_23_16=0x00, .base_15_0=0x0000, .limit_15_0=0xFFFF };

    // Descriptor de codigo de nivel 3 (usuario)
    descriptor_table CS_SEL3 =
     { .base_31_24=0x00, .g=0b1, .d_b=0b1, .l=0b0, .avl=0b0, .limit19_16=0b1111,
     .p=0b1, .dpl=0b11, .s=0b1, .type=0b1001, .base_23_16=0x00, .base_15_0=0x0000, .limit_15_0=0xFFFF };

    // Descriptor de datos de nivel 3 (usuario)
    descriptor_table DS_SEL3 =
     { .base_31_24=0x00, .g=0b1, .d_b=0b1, .l=0b0, .avl=0b0, .limit19_16=0b1111,
     .p=0b1, .dpl=0b11, .s=0b1, .type=0b0010, .base_23_16=0x00, .base_15_0=0x0000, .limit_15_0=0xFFFF };

    // Tss basica utilizada solo para que el procesador pueda obtener la pila de nivel 0 al saltar de privilegio
    descriptor_table BASIC_TSS =
     { .base_31_24=0x04, .g=0b0, .d_b=0b1, .l=0b0, .avl=0b0, .limit19_16=0b0000,
     .p=0b1, .dpl=0b00, .s=0b0, .type=0b1001, .base_23_16=0x30, .base_15_0=0x0000, .limit_15_0=0x0067 };

    // Se agregan los descriptores a la tabla del sistema en el orden esperado por los index's del linker script que usa el codigo mas adelante
    addGlobalDescriptorTable(nullDT);
    addGlobalDescriptorTable(CS_SEL0);
    addGlobalDescriptorTable(DS_SEL0);
    addGlobalDescriptorTable(CS_SEL3);
    addGlobalDescriptorTable(DS_SEL3);
    addGlobalDescriptorTable(BASIC_TSS);
    
    // Seteo el valor de idtr en base a la cantidad de descriptores que agregue
    // y el tamano de la GDT
    *dtrLimit = descriptor_tables_index*8-1;
    dtPointer = (char *)&_idtr;
    dtrBase = (int *) (dtPointer+2);
    *dtrBase = (int)((char*)&__SYS_TABLES_START+*dtrLimit+1);

    return;
}
// Rutina encargada de inicializar todos los descriptores de las interrupciones
// Para facilitar el desarrollo se creo con macros de assembler una funcion para cada excepcion del procesador (todas del mismo tamano
// y solo ponen un breackpoint y su numero de interrupcion en dx)
// Se hace un setup inicial de todas las IRQ default (en el for) y luego se sobre-escriben las utilizadas
// Pasada la etapa de desarrollo se puede sacar el for y solo setear las interrupciones utilizadas y que pueden ocurrir indeseadamente,
// por ejemplo todas las del PIC, a una funcion guard.
void __attribute__ ((section (".set_dt")))SetInterruptDescriptionTables()
{
    int i = 0;
    byte* dtPointer = (byte*)&_idtr;
    short *dtrLimit = (short*)dtPointer;
    int gdtSize = descriptor_tables_index;
    for(; i<256; i++)
    {
        addInterruptDescriptorTable(descriptor_tables_index+i, (int)((char*)__interrupHandler__0 + i*default_handler_code_size), (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00);
    }
    addInterruptDescriptorTable(descriptor_tables_index + 0x00, (int)__devide_zeroIRQ__, (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00); // Rewrite Devide by Zero Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x06, (int)__invalid_opcodeIRQ__, (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00); // Rewrite GP Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x08, (int)__double_faultIRQ__, (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00); // Rewrite GP Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x0D, (int)__general_protectionIRQ__, (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00); // Rewrite GP Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x0E, (int)__pageFaultIRQ__, (int)&__CS_SEL_32_INDEX, 0b1111, 0x00, 1, 0x00); // Rewrite Page Fault Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x20, (int)__systickIRQ__, (int)&__CS_SEL_32_INDEX, 0b1110, 0x00, 1, 0x00); // Rewrite for Timer Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x21, (int)__keyboardIRQ__, (int)&__CS_SEL_32_INDEX, 0b1110, 0x00, 1, 0x00); // Rewrite for Keyboard Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x07, (int)__deviceNotAvailable, (int)&__CS_SEL_32_INDEX, 0b1110, 0x00, 1, 0x00); // Rewrite for Keyboard Handler
    addInterruptDescriptorTable(descriptor_tables_index + 0x80, (int)__sys_call, (int)&__CS_SEL_32_INDEX, 0b1111, 0x3, 1, 0x00); // Rewrite for Sys Call con nivel de privilegio 3
    descriptor_tables_index += i-1;
    *dtrLimit = (descriptor_tables_index - gdtSize) * 8 -1;
}

// funciones que se encargan de todo el trabajo a nivel memoria para configurar los descriptores y aumentar el index de la tabla del sistema:

void __attribute__ ((section (".set_dt")))addGlobalDescriptorTable(descriptor_table newTable)
{
    char* address = (char *)&__SYS_TABLES_START; // me corro de a 64 bits.
    address +=  (descriptor_tables_index*8);
    *address = (char) newTable.limit_15_0;
    address++;
    *address = (char) newTable.limit_15_0>>8;
    address++;
    *address = (char) newTable.base_15_0;
    address++;
    *address = (char) newTable.base_15_0>>8;
    address++;
    *address = (char) newTable.base_23_16;
    address++;
    *address = (char)newTable.type | (char)(newTable.s<<4) | (char)(newTable.dpl<<5) | (char)(newTable.p<<7);
    address++;
    *address = (char)newTable.limit19_16 | (char)(newTable.avl<<4) | (char)(newTable.l<<5) | (char)(newTable.d_b<<6) | (char)(newTable.g<<7);
    address++;
    *address = (char)newTable.base_31_24;

    descriptor_tables_index++;
    return;
}

void __attribute__ ((section (".set_dt")))addInterruptDescriptorTable(int index, int offset, int segmentSelector, char type, char dpl, char p, char ist)
{
    char* address = (char*)&__SYS_TABLES_START;
    address +=  (index*8);
    *address = (char) offset;
    address++;
    *address = (char) (offset>>8);
    address++;
    *address = (char) segmentSelector;
    address++;
    *address = (char) (segmentSelector>>8);
    address++;
    *address = (char) (0x03 & ist);
    address++;
    *address = (char) (type | dpl<< 5 | p<<7); // el bit S siempre en 0
    address++;
    *address = (char) (offset>>16);
    address++;
    *address = (char) (offset>>24);

    index++;
    return;
}
