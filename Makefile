.PHONY: all

all: floppy.img stillalive.rom

CC = gcc

CFLAGS = -m16 -c -fno-PIE -ffreestanding -nostartfiles -nostdlib -std=c11 -Os

OBJ = main.o    \
	misc.o \
	music.o \
	data.o \
	math.o \
	ivt.o \
	memaccess.o \
	graphics.o \
	disk.o \

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	
unreal_mode.o: unreal_mode.asm
	nasm -f elf32 $< -o $@

entry.o: entry.S
	as --32 -o entry.o entry.S

stillalive.rom: loader.bin demo.bin addchecksum
	dd if=/dev/zero of=$@ bs=1 count=32768
	dd if=loader.bin of=$@ bs=1 conv=notrunc
	dd if=demo.bin of=$@ bs=1 conv=notrunc seek=512
	./addchecksum $@ || rm $@

loader.bin: loader.asm
	nasm $< -fbin -o $@

addchecksum: addchecksum.c
	gcc -o $@ $< -Wall

floppy.img: boot.bin demo.bin
	mformat -i floppy.img -f 1440 -C -v FLOPPY -B boot.bin
	mcopy -i floppy.img demo.bin ::/

demo.bin: $(OBJ) entry.o unreal_mode.o
	ld -z noexecstack -m elf_i386 -o main.elf -T linker.ld entry.o unreal_mode.o $(OBJ)
	objcopy -O binary main.elf demo.bin

boot.bin: bootloader.asm
	nasm $< -f bin -o $@ 

.PHONY: clean
clean:
	rm -rf *.rom *.o *.elf *.bin *.img

.PHONY: runqe
runqemu: floppy.img
	qemu-system-i386 -fda ./floppy.img -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -debugcon stdio

runrom: stillalive.rom
	qemu-system-i386 -net none -option-rom stillalive.rom -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

