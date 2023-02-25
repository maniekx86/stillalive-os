.PHONY: all

all: floppy.img stillalive.rom

SRC = data.c
SRC += disk.c
SRC += graphics.c
SRC += ivt.c
SRC += main.c
SRC += math.c
SRC += memaccess.c
SRC += misc.c
SRC += linker.ld
SRC += entry.S

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

demo.bin: $(SRC)
	as -ggdb3 --32 -o entry.o entry.S
	gcc -m16 -c -ggdb3 -fno-PIE -ffreestanding -nostartfiles -nostdlib -o main.o -std=c11 main.c 
	ld -m elf_i386 -o main.elf -T linker.ld entry.o main.o
	objcopy -O binary main.elf demo.bin

boot.bin: bootloader.asm
	nasm $< -f bin -o $@ 

.PHONY: clean
clean:
	rm -rf *.rom *.o *.elf *.bin *.img

.PHONY: runqe
runqemu: floppy.img
	qemu-system-i386 -fda ./floppy.img -soundhw pcspk

runrom: stillalive.rom
	qemu-system-i386  -net none -option-rom stillalive.rom -soundhw pcspk
