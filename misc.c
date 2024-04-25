// misc.c
/*
	Misc and low level functions

	Written by maniek86 2022 (c) 
*/
#include "typedef.h"
#include "misc.h"

extern u32 time_ms;

void my_reverse(char str[], int len)
{
    u16 start, end;
    char temp;
    for(start=0, end=len-1; start < end; start++, end--) {
        temp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = temp;
    }
}
 
char* itoa(int num, char* str, int base)
{
    u16 i = 0;
    u8 isNegative = 0;
  
    if (num == 0) {
        str[i] = '0';
        str[i + 1] = '\0';
        return str;
    }

    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
  
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num/base;
    }

    if (isNegative==1){
        str[i++] = '-';
    }
  
    str[i] = '\0';
 
    my_reverse(str, i);
  
    return str;
}

void __attribute__((optimize("O0"))) sleep(u32 ms) {
	u32 start=time_ms;
	while(1) {
	if(start+ms<time_ms) break;
	}
}

void qemu_debugcon(char* str) {
    while(*str) {
        outb(0xe9,*str); // write to debug port
        str++;
    }
    outb(0xe9,'\n');
}

void hide_cursor() {
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void* memset(void* bufptr, int value, u32 size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (u32 i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}
