# stillalive-os
Portal ending "Still Alive" as an operating system or BIOS extension

[YouTube Video](https://www.youtube.com/watch?v=sB0hh7UAAfA)

![image](https://user-images.githubusercontent.com/104389805/210117492-dd44cddb-3780-4c4e-9c47-db19438f8243.png)

## Notes:
- Might be glitchy at times
- ASCII art draws slowly on some systems and may pause the music
- Does not include staff credits
- Requires a PC speaker, VESA GPU, and Intel 386 compatible CPU or higher
- Release files: precompiled image `demo.bin` requires a boot loader; `floppy.img` is a 1.44 MB floppy image (also works when flashed to USB)
- Code is messy
- Runs as an x86 real mode operating system

## Compiling
`nasm`, `gcc`, and `mtools` are required to compile.
Simply run `make` to compile everything (bootloader, code, floppy image, and BIOS ROM).

Use the `make floppy.img` command to assemble the `floppy.img` image.

Use the `make stillalive.rom` command to build the BIOS ROM image file `stillalive.rom`.

## Running
Tests: `make runqemu` launches QEMU with PC speaker support.
To run on a real device, simply flash the image onto a floppy or even a USB drive.

Or use the `make runrom` command to run the BIOS extension in emulation.

## Extra

Special thanks to [@dlinyj](https://github.com/dlinyj) for helping and testing!

_If you're looking for an x86 operating system template to build your own project like this, you can refer to my other GitHub project, BruhOS, which this project is initially based on._
