# stillalive-os
Portal ending "Still Alive" as operating system

https://www.youtube.com/watch?v=sB0hh7UAAfA

![image](https://user-images.githubusercontent.com/104389805/210117492-dd44cddb-3780-4c4e-9c47-db19438f8243.png)



## Notes:
- Might be sometimes glitchy
- ASCII arts draw slow on some system and they pause music
- Does not include staff credits
- Requires PC speaker, VESA GPU and i386/higher
- Release files: precompiled image demo.bin requires boot loader, floppy.img is 1.44 MB floppy image (works when flashed on USB too)
- Code is messy
- Runs as x86 real mode opearting system

## Compiling
nasm, gcc and mtools are required to compile.
Simply run `make` to compile everything (bootloader, code and image)

## Running
Tests: `make runqemu` launches qemu with pcspeaker.
To run on a real device simply flash image on floppy or even USB drive.


_Please don't use this as x86 operating system template. For template you can see my another project - BruhOS that this project is based on (outdated)_
