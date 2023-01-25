.PHONY	:	clean

BINPATH = ./bin/
DOCPATH = ./doc/
INCPATH = ./inc/
LIBPATH = ./lib/
SRCPATH = ./src/



AC	=	nasm
CC	=	gcc
AFLAGS	=	-f elf32 -i $(INCPATH)
CFLAGS	=	-c  -m32 -fno-stack-protector -fno-asynchronous-unwind-tables -Wall -fno-PIC 
LDFLAGS =	-z max-page-size=0x01000 --oformat=binary -m elf_i386

LDSCRIPT=	linker.ld
ODFLAGS	=	-CprsSx --prefix-addresses
OBJS	=	$(BINPATH)reset.elf $(BINPATH)init16.elf $(BINPATH)gateA20.elf $(BINPATH)init_gdt.elf $(BINPATH)init32.elf \
			$(BINPATH)descriptorTablesHandler.elf $(BINPATH)functions_ROM.elf $(BINPATH)main.elf $(BINPATH)setDescriptorTables.elf \
			$(BINPATH)PIC_setup.elf $(BINPATH)keyboardTask.elf $(BINPATH)uninitialised_data.elf $(BINPATH)timer_setup.elf $(BINPATH)keyboard_setup.elf \
			$(BINPATH)scheduler.elf $(BINPATH)init_VGA.elf $(BINPATH)monitorHandler.elf $(BINPATH)addNumbersTask.elf $(BINPATH)initialised_data.elf \
			$(BINPATH)pagingHandler.elf $(BINPATH)IRQ_handlers.elf $(BINPATH)dispatcher.elf $(BINPATH)addNumbersTask2.elf $(BINPATH)idleTask.elf $(BINPATH)mmxAdd.elf \
			$(BINPATH)sseAdd.elf $(BINPATH)task1SIMD.elf $(BINPATH)task2SIMD.elf $(BINPATH)sys_calls_api.elf $(BINPATH)keyboardInputHandler.elf
SRC_NAME= bios

debug: AFLAGS +=  -dDEBUG
debug: CFLAGS +=  -DDEBUG

debug: all

all: $(BINPATH)$(SRC_NAME).bin $(BINPATH)$(SRC_NAME).elf
	mkdir -p doc
	hexdump -C $(BINPATH)$(SRC_NAME).bin > $(DOCPATH)$(SRC_NAME)_hexdump.txt
	objdump -C $(ODFLAGS) $(BINPATH)$(SRC_NAME).elf > $(DOCPATH)$(SRC_NAME)_objdump.txt
	readelf -a $(BINPATH)$(SRC_NAME).elf > $(DOCPATH)$(SRC_NAME)_readelf.txt


$(BINPATH)$(SRC_NAME).bin: $(OBJS)
	ld $(LDFLAGS) -T $(LDSCRIPT) $(OBJS) -o $@ -Map $(SRC_NAME).map

$(BINPATH)$(SRC_NAME).elf: $(OBJS)
	ld -z max-page-size=0x01000 -m elf_i386 -T $(LDSCRIPT) $(OBJS) -o $@

$(BINPATH)reset.elf: $(SRCPATH)16bits/reset.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)init16.elf: $(SRCPATH)16bits/init16.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)init_VGA.elf: $(SRCPATH)16bits/init_VGA.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)gateA20.elf: $(SRCPATH)16bits/gateA20.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)task1SIMD.elf: $(SRCPATH)16bits/task1SIMD.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)task2SIMD.elf: $(SRCPATH)16bits/task2SIMD.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)protectedModeInit.elf: $(SRCPATH)16bits/protectedModeInit.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)$(SRC_NAME).bin: $(OBJS)
	ld $(LDFLAGS) -T $(LDSCRIPT) $(OBJS) -o $@ -Map $(SRC_NAME).map

$(BINPATH)init_gdt.elf: $(SRCPATH)16bits/init_gdt.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)init32.elf: $(SRCPATH)32bits/init/init32.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)setDescriptorTables.elf: $(SRCPATH)32bits/init/setDescriptorTables.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)PIC_setup.elf: $(SRCPATH)32bits/init/PIC_setup.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)uninitialised_data.elf: $(SRCPATH)32bits/data/uninitialised_data.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)initialised_data.elf: $(SRCPATH)32bits/data/initialised_data.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)descriptorTablesHandler.elf: $(SRCPATH)32bits/handlers/descriptorTablesHandler.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)main.elf: $(SRCPATH)32bits/kernel/main.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)functions_ROM.elf: $(SRCPATH)32bits/init/functions_ROM.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)keyboardTask.elf: $(SRCPATH)32bits/tasks/keyboardTask.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)timer_setup.elf: $(SRCPATH)32bits/init/timer_setup.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)keyboard_setup.elf: $(SRCPATH)32bits/init/keyboard_setup.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)IRQ_handlers.elf: $(SRCPATH)32bits/handlers/IRQ_handlers.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)keyboardInputHandler.elf: $(SRCPATH)32bits/handlers/keyboardInputHandler.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)mmxAdd.elf: $(SRCPATH)32bits/tasks/mmxAdd.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)sseAdd.elf: $(SRCPATH)32bits/tasks/sseAdd.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)sys_calls_api.elf: $(SRCPATH)32bits/kernel/sys_calls_api.s
	mkdir -p bin
	$(AC) $(AFLAGS) $< -o $@ -l $@.lst

$(BINPATH)scheduler.elf: $(SRCPATH)32bits/kernel/scheduler.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)monitorHandler.elf: $(SRCPATH)32bits/handlers/monitorHandler.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)addNumbersTask.elf: $(SRCPATH)32bits/tasks/addNumbersTask.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)pagingHandler.elf: $(SRCPATH)32bits/handlers/pagingHandler.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)dispatcher.elf: $(SRCPATH)32bits/kernel/dispatcher.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)addNumbersTask2.elf: $(SRCPATH)32bits/tasks/addNumbersTask2.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

$(BINPATH)idleTask.elf: $(SRCPATH)32bits/tasks/idleTask.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

bochs:
	bochs -f bochsrc -q

clean:
	rm $(BINPATH)*.bin $(BINPATH)*.elf $(BINPATH)*.lst $(DOCPATH)*.txt *.map