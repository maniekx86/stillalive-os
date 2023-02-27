// graphics.c
/*
	Graphics related functions

	Written by maniek86 2022 (c) 
*/

#include "memaccess.h"
#include "typedef.h"
#include "graphics.h"

static char *vgamem = (u8*)0xB000;

// text mode functions

void bios_printchar(char chr, u8 color) {
	__asm__(
            "int $0x10"
            :
            : "a"((0x0e << 8) | chr), "b"((0 << 8) | color));
}
void bios_print(const char *s,u8 color) {
    while(*s) {
        if(*s=='\n') {
            newline();
        } else {
            bios_printchar(*s,color);
        }
        s++;
    }
}


void setchar(u8 x,u8 y,u8 c,u8 color) {
    dispchar((color << 8) | c,(y*80+x)*2);
}

void setcursor(u8 x, u8 y) {
    __asm__(
        "int $0x10"
        :
        : "a"((0x02 << 8) | 0), "d"((y << 8) | x), "b"(0));
}

void getcursor(u8 *x, u8 *y) {
    uint16_t out;
    __asm__(
        "int $0x10"
        : "=d"(out)
        : "a"((0x03 << 8) | 0), "b"(0));
    *x = (out >> 0) & 0xFF;
    *y = (out >> 8) & 0xFF;
}


void newline() {
    u8 x,y;
	getcursor(&x,&y);
	if(y>23) {
        u32 i=0;
        bios_printchar('\n',0);
		setcursor(0,23);
	} else {
        setcursor(0,y+1);
    }
}

void setvideomode(u8 mode) {
    __asm__(
        "int $0x10"
        :
        : "a"((0x00 << 8) | mode));
}
