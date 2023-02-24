.PHONY: all
all: floppy.img

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
