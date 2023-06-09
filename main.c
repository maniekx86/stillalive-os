// main.c
/*
	Main file

	Written by maniek86 2023 (c) 
*/
#include "typedef.h"
#include "misc.h"
#include "music.h"
#include "data.h"
#include "math.h"
#include "ivt.h"
#include "graphics.h"
#include "memaccess.h"
#include "disk.h"

u32 time_ms;

u8 playmusic = 0;
u8 playendmusic = 0;

u16 nexttick = 0;
int32_t musicpos = 0;
u8 theend = 0;

int textn = -1;
int textndelay = 0;
int currenttext = 0;

u8 cursorblinkx = 2;
u8 cursorblinky = 3;
u8 cursorblinks = 0;
u8 cursorblinken = 0;
u8 cursorblinktick = 0;
u16 cursort = 0;

u8 ascii_charmap[16][16]; // We need: . , - : ; / H @ M + X = # % $ for ascii arts

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

	ivt_set_callback(&pic_handler,8); //8-8=irq 0
}

void drawascii(const char *s,u8 color,u8 cx,u8 cy) {
	u8 dontpe=0;
	u16 tx,ty;
	u16 ttx, tty;

	if(playmusic==0) {
		dontpe=1;
	}
	playmusic=0;

	for(ty=0;ty<16;ty++) {
		for(tx=0;tx<40;tx++) { // Clear last ascii art
			disppixel(0,(ty+cy*16)*100+tx+cx);
		}
	}

	ty=cy;
	tx=0;

    while(*s) {
        if(*s=='\n') {
	    ty++;
            tx=0;
            vga_set_plane(1);
            for(tty=0;tty<16;tty++) {
		for(ttx=0;ttx<40;ttx++) { 
		   disppixel(0,(tty+ty*16)*100+ttx+cx);
		}
            }
            vga_set_plane(2);
            for(tty=0;tty<16;tty++) {
		for(ttx=0;ttx<40;ttx++) { 
		   disppixel(0,(tty+ty*16)*100+ttx+cx);
		}
            }
        } else if(*s==' ') {
		tx++;
		} else {
			u8 charindex;
			// looks bad, but it should be fastest
			switch(*s) {
				case '.': charindex=0; break;
				case ',': charindex=1; break;
				case '-': charindex=2; break;
				case ':': charindex=3; break;
				case ';': charindex=4; break;
				case '/': charindex=5; break;
				case 'H': charindex=6; break;
				case '@': charindex=7; break;
				case 'M': charindex=8; break;
				case '+': charindex=9; break;
				case 'X': charindex=10; break;
				case '=': charindex=11; break;
				case '#': charindex=12; break;
				case '%': charindex=13; break;
				case '$': charindex=14; break;
				default: charindex=15; break;
				
			}
			vga_set_plane(1);
            for(int b=0;b<16;b++) {
				disppixel(ascii_charmap[b][charindex],(ty*16+b)*100+cx+tx);
			}
			vga_set_plane(2);
            for(int b=0;b<16;b++) {
				disppixel(ascii_charmap[b][charindex],(ty*16+b)*100+cx+tx);
			}
			tx++;
        }
        s++;
    }
	vga_set_plane(0);
	if(dontpe==0) playmusic=1;
}

void prepare_ascii() { // Render characters to ascii_charmap[] before demo
	char to_render[] = {'.',',','-',':',';','/','H','@','M','+','X','=','#','%','$',' '};
	for(int i=0;i<sizeof(to_render);i++) {
		setcursor(0,0);	
		bios_printchar(to_render[i],15);
		for(int b=0;b<16;b++) {
			ascii_charmap[b][i]=getpixel(b*100);
		}
	}
}


void clear() {
	for(int ix=0;ix<47;ix++) {
		for(int iy=0;iy<33*16;iy++) {
			disppixel(0,(iy+16)*100+1+ix);
		}
	}
}

void typeslow(const char *s, u8 time) {
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
		if(s[0]=='0') drawascii(text_ascii_0,6,55,16);
		if(s[0]=='1') drawascii(text_ascii_1,6,55,16);
		if(s[0]=='2') drawascii(text_ascii_2,6,55,16);
		if(s[0]=='3') drawascii(text_ascii_3,6,55,16);
		if(s[0]=='4') drawascii(text_ascii_4,6,55,16);
		if(s[0]=='5') drawascii(text_ascii_5,6,55,16);
		if(s[0]=='6') drawascii(text_ascii_6,6,55,16);
		if(s[0]=='7') drawascii(text_ascii_7,6,55,16);
		if(s[0]=='8') drawascii(text_ascii_8,6,55,16);
		if(s[0]=='9') drawascii(text_ascii_9,6,55,16);
		cursorblinktick=1;
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

	prepare_ascii();

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
