// main.c
/*
	Main file

	Written by maniek86 2023 (c) 
*/
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

u32 time_ms;

#include "misc.c"
#include "ivt.c"
#include "math.c"
#include "graphics.c"
#include "data.c"
#include "disk.c"

u8 playmusic=0;
u8 playendmusic=0;

void nmi_handler() {
	__asm("pusha");

	__asm("nop;nop;nop;nop");
	outb(0x20, 0x20);
	__asm("popa;leave;iret");
}

char tmpstr[10];

void keyboard_hanlder() {
	__asm("pusha");
	
	u8 key=inb(0x60);
	u8 press=0;
	if(key>127) { // key press
		press=1;
		qemu_debugcon("press");
	}
	key=key%128;
	itoa(key,tmpstr,10);
	qemu_debugcon(tmpstr);


	
	__asm("nop;nop;nop;nop");
	outb(0x20, 0x20);
	__asm("popa;leave;iret");
}

u8 cursorblinkx=2;
u8 cursorblinky=3;
u8 cursorblinks=0;
u8 cursorblinken=0;
u8 cursorblinktick=0;
u16 cursort=0;

void pic_handler() { // PIC is set to 1000 Hz
	__asm("pusha");

	time_ms=time_ms+1;
	if(cursorblinktick) cursort++;
	music_handle();
	music_handle2();

	u8 lx,ly;

	if(cursort>=500) {
		cursort=0;
		getcursor(&lx,&ly);
		if(cursorblinks==1) {
			if(cursorblinken==1) {
				setcursor(cursorblinkx,cursorblinky);
				bios_printchar('_',6);
			}
			cursorblinks=0;
		} else {
			if(cursorblinken==1) {
				setcursor(cursorblinkx,cursorblinky);
				bios_printchar(' ',6);
			}
			cursorblinks=1;
		}
		setcursor(lx,ly);

	}

	__asm("nop;nop;nop;nop");
	outb(0x20, 0x20);
	__asm("popa;leave;iret");
}

void interrupt_setup() {
	set_timer_hz(1000);

	ivt_set_callback(&nmi_handler,2);
	ivt_set_callback(&pic_handler,8); //8-8=irq 0
	ivt_set_callback(&keyboard_hanlder,9); //9-8=irq 1

}

void csetpix(u8 x,u8 y,u8 c) {
	dispchar(chr(c,14),((y*80)+x)*2);
}



char intstr[18]; // generic string for displaying numbers

void drawascii(const char *s,u8 color) {
	u8 dontpe=0;
	if(playmusic==0) {
		dontpe=1;
	}
	playmusic=0;
	u8 cx,cy;
	getcursor(&cx,&cy);
	u8 tx,ty;
	for(int ix=0;ix<40;ix++) {
		for(int iy=0;iy<16;iy++) {
			disppixel(0,(iy+cy*16)*100+cx+ix);
		}
	}
	setcursor(cx,cy);
    while(*s) {
        if(*s=='\n') {
			cy++;
            setcursor(cx,cy);
			for(int ix=0;ix<40;ix++) {
				for(int iy=0;iy<16;iy++) {
					disppixel(0,(iy+cy*16)*100+cx+ix);
				}
			}
        } else if(*s==' ') {
			getcursor(&tx,&ty);
			setcursor(tx+1,ty);
		} else {
            bios_printchar(*s,color);
        }
        s++;
    }
	if(dontpe==0) playmusic=1;
}


int NOTEFREQ[]= // FREQ OF NOTES //midi message 12-119
{16,17,18,19,20,21,23,24,25,27,29,30, //0
32,34,36,38,41,43,46,49,51,55,58,61,  // 1
65,69,73,77,82,87,92,98,103,110,116,123,//2
130,138,146,155,164,174,185,196,207,220,233,246,//3
261,277,293,311,329,349,369,392,415,440,466,493, //4
523,554,587,622,659,698,739,783,830,880,932,987, //5
1046,1108,1174,1244,1318,1396,1479,1567,1661,1760,1864,1975,//6
2093,2217,2349,2489,2637,2793,2959,3135,3322,3520,3729,3951, //7
4186,4434,4698,4678,5274,5587,5919,6271,6644,7040,7458,7902}; //8

u16 nexttick=0;
int32_t musicpos=0;


int textn=-1;
int textndelay=0;
int currenttext=0;
u8 theend=0;

void music_handle() {
	if(playmusic==0) return;
	nexttick++;
	if(nexttick<63) return; // 120 bpm
	nexttick=0;
	if(musicdata[musicpos]==0) {
		if(musicdata[musicpos+1]!=0) {
			nosound();
		}
	} else if(musicdata[musicpos]==255) {
		nosound();
		outb(0x80,0);
	} else if(musicdata[musicpos]==254) {
		nosound();
		outb(0x80,255);
		playmusic=0;
	} else {
		if(musicdata[musicpos]<100) play_sound(NOTEFREQ[musicdata[musicpos]]);
		outb(0x80,musicdata[musicpos]);
	}
	if(theend==0&&lyricsync[musicpos]!=0) {
		if(textn!=-1) {
			qemu_debugcon("WORD SKIPPED I HATE MY LIFE");
		}
		if(lyricsync[musicpos]==255) {
			theend=1;
		}
		textn=currenttext;
		textndelay=lyricsync[musicpos];
		currenttext++;
	}
	musicpos++;
}

void music_handle2() {
	if(playendmusic==0) return;
	nexttick++;
	if(nexttick<63) return; // 120 bpm
	nexttick=0;
	if(musicdata2[musicpos]==0) {
		if(musicdata2[musicpos+1]!=0) {
			nosound();
		}
	} else if(musicdata2[musicpos]==255) {
		nosound();
		outb(0x80,255);
	} else if(musicdata2[musicpos]==254) {
		musicpos=-1;
		outb(0x80,254);
	} else {
		play_sound(NOTEFREQ[musicdata2[musicpos]]);
		outb(0x80,musicdata2[musicpos]);
	}
	musicpos++;
}

void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
        //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}
 
void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 
 	outb(0x61, tmp);
}

void clear() {
	for(int ix=0;ix<47;ix++) {
		for(int iy=0;iy<33*16;iy++) {
			disppixel(0,(iy+16)*100+1+ix);
		}
	}
}

void typeslow(char *s,u8 time) {	
	u8 cx,cy;
	u8 donewline=0;
	u8 skipfirstchar=0;
	u8 skipspace=0;

	getcursor(&cx,&cy);

	setcursor(cursorblinkx,cursorblinky);
	bios_printchar(' ',6);
	setcursor(cx,cy);

	if(s[0]=='{') { // 2
		cursorblinken=0;
		clear();
		setcursor(2,1);
		cursorblinkx=2;
		cursorblinky=1;
		cursorblinken=1;
		typeslow(text_t2_0,50);
		typeslow(text_t2_1,50);
		typeslow(text_nw,1);
		typeslow(text_t2_2,50);
		typeslow(text_nw,1);
		return;
	}
	if(s[0]=='}') { // 3
		cursorblinken=0;
		clear();
		setcursor(2,1);
		cursorblinkx=2;
		cursorblinky=1;
		cursorblinken=1;
		typeslow(text_t3_0,25);
		typeslow(text_t3_1,25);
		typeslow(text_nw,100);
		typeslow(text_t2_2,80);
		typeslow(text_nw,1);
		return;
	}
	if(s[0]=='|') { // 4
		cursorblinken=0;
		clear();
		setcursor(2,4);
		cursorblinkx=2;
		cursorblinky=4;
		cursorblinken=1;
		return;
	}

	if(s[0]=='[') {
		donewline=1;
		skipfirstchar=1;
	}
	if(s[0]==']') {
		skipfirstchar=1;
		skipspace=1;
	}
	if(s[0]=='0'||s[0]=='1'||s[0]=='2'||s[0]=='3'||s[0]=='4'||s[0]=='5'||s[0]=='6'||s[0]=='7'||s[0]=='8'||s[0]=='9') {
		skipfirstchar=1;
		cursorblinktick=0;
		setcursor(55,16);
		if(s[0]=='0') drawascii(text_ascii_0,6);
		if(s[0]=='1') drawascii(text_ascii_1,6);
		if(s[0]=='2') drawascii(text_ascii_2,6);
		if(s[0]=='3') drawascii(text_ascii_3,6);
		if(s[0]=='4') drawascii(text_ascii_4,6);
		if(s[0]=='5') drawascii(text_ascii_5,6);
		if(s[0]=='6') drawascii(text_ascii_6,6);
		if(s[0]=='7') drawascii(text_ascii_7,6);
		if(s[0]=='8') drawascii(text_ascii_8,6);
		if(s[0]=='9') drawascii(text_ascii_9,6);
		cursorblinktick=1;
		setcursor(cx,cy);
	}
	int i=0;
    while(*s) {
		if(skipfirstchar&&i==0) goto t_next;
		u8 tx,ty;
		getcursor(&tx,&ty);
		cursorblinkx=tx+1;
		cursorblinky=ty;
        bios_printchar(*s,6);
		if(cursorblinks==0) {
			bios_printchar('_',6);
			setcursor(tx+1,ty);
		}
		sleep(time);

		t_next:
        s++;
		i++;
    }
	if(donewline) {
		cy++;
		bios_printchar(' ',6 );
		cursorblinkx=2;
		cursorblinky=cy;
        setcursor(1,cy);
	}
	if(!skipspace) bios_printchar(' ',6);
}

void main(void) {
	u8 cx,cy;
	bios_print(text_startup,7);
	
	__asm("cli");
	interrupt_setup();
	__asm("sti");
	sleep(3000);
	resetDisk(0);
	setvideomode(0x6A);
	__asm__(
        	"int $0x10"
        	:
        	: "a"(0x1124), "b"(03)); // http://www.techhelpmanual.com/165-int_10h_1124h__setup_rom_8x16_font_for_graphics_mode.html
	hide_cursor();

	setcursor(0,0);
	for(u8 x=0;x<50;x++) {
		bios_printchar('-',6);
	}
	bios_printchar(' ',6);
	for(u8 x=0;x<48;x++) {
		bios_printchar('-',6);
	}

	for(u8 y=1;y<34;y++) {
		setcursor(0,y);
		bios_printchar('|',6);
		setcursor(49,y);
		bios_printchar('|',6);
		if(y<16) {
			setcursor(50,y);
			bios_printchar('|',6);
			setcursor(99,y);
			bios_printchar('|',6);
		}
	}

	setcursor(0,34);
	for(u8 x=0;x<50;x++) {
		bios_printchar('-',6);
	}

	setcursor(51,15);
	for(u8 x=0;x<48;x++) {
		bios_printchar('-',6);
	}

	newline();
	cursorblinken=1;
	cursorblinktick=1;
	cursorblinkx=2;
	cursorblinky=1;
	setcursor(2,1);

	typeslow(text_t1_0,60);
	typeslow(text_t1_1,60);
	typeslow(text_nw,1);
	typeslow(text_nw,1);
	playmusic=1;

	char buf[13];
	u8 ni=0;
    while (1) {
		if(textn!=-1) {
			int tmp=textn;
			textn=-1;
			typeslow(lyrics[tmp],textndelay);
			if(textn!=-1) {
				playmusic=0;
				typeslow(lyrics[textn],textndelay);
				playmusic=1;
				textn=-1;
			}
			
		}
		if(theend==2) {
			sleep(10000);
			break;
		}
		if(theend==1) {
			theend=2;
			cursorblinken=0;
			clear();
			setcursor(2,1);
			cursorblinkx=2;
			cursorblinky=1;
			cursorblinken=1;
		}
    }
	cursorblinken=0;
	setvideomode(0);
	setcursor(0,0);
	bios_print("THANK YOU FOR PARTICIPATING\nIN THIS\nENRICHMENT CENTER ACTIVITY!!",7);
	musicpos=0;
	nexttick=0;
	playendmusic=1;
	while(1) {

	}
}
