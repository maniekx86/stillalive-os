#!/usr/bin/env bash
set -eux
as -ggdb3 --32 -o entry.o entry.S
gcc -m16 -c -ggdb3 -fno-PIE -ffreestanding -nostartfiles -nostdlib -o main.o -std=c11 main.c 
ld -m elf_i386 -o main.elf -T linker.ld entry.o main.o
objcopy -O binary main.elf demo.bin
# create floppy image 1.44 MB
mformat -i floppy.img -f 1440 -C -v FLOPPY -B boot.bin

# copy operating system to floppy image on fat12 filesystem
sudo mount -t msdos -o loop floppy.img /mnt
sudo cp demo.bin /mnt/demo.bin
sudo umount /mnt
# create bootloader on image




