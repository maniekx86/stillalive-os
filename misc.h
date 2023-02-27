// misc.h
/*
	Misc and low level functions

	Written by maniek86 2022 (c) 
*/

#ifndef __MISC_H___
#define __MISC_H___
#include "typedef.h"

static inline void outb(u16 port, u8 val) {
    __asm volatile("outb %0, %1"
                   :
                   : "a"(val), "Nd"(port));
}

static inline u8 inb(u16 port)
{
    u8 ret;
    __asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void my_reverse(char str[], int len);
char* itoa(int num, char* str, int base);
void sleep(u32 ms);
void qemu_debugcon(char* str);
void hide_cursor(void);
void* memset(void* bufptr, int value, u32 size);
#endif
