// main.c
/*
	Main file

	Written by maniek86 2024 (c) 
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

extern void _gounreal(); // Enable A20 gate and go unreal mode
extern uint8_t _getmem(u32);
extern uint16_t _getmemw(u32);


u32 time_ms;

u8 graphics_mode = 0; // 0 - legacy (4bpp), 1 - modern (16bpp)
 

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

u8 ascii_charmap[16][16]; // We need: . , - : ; / H @ M + X = # % $ for ascii arts (4bpp)


// printchar and print for the demo routines (print properly no matter if 4bpp or 16bpp fb)
u8 cur_x=0;
u8 cur_y=0;
void setcursor(u8 x, u8 y) {
    if(graphics_mode==0) { // text mode/4bpp
        vga_setcursor(x, y);
    } else { // 16bpp
        cur_x = x;
        cur_y = y;
    }
}
void getcursor(u8 *x, u8 *y) {
    if(graphics_mode==0) { // text mode/4bpp
		vga_getcursor(x,y);
    } else { // 16bpp
        *x = cur_x;
        *y = cur_y;
    }
}
void newline() {
    if(graphics_mode==0) { // text mode/4bpp
        vga_newline();
    } else {
        cur_x=0;
        cur_y++;
    }
}
void printchar(char c) {
    if(graphics_mode==0) { // 4bpp
        bios_printchar(c,6);
    } else { // 16bpp
        for(int y=0;y<16;y++) {
            u8 line = VGA_FONT[c*16+y];
            for(int x=0;x<8;x++) {
                u16 color=0;
                if((line>>(7-x)) & 1) {
                    color=get_orange_color();                
                }
	            disppixel_16bpp(color, cur_x*8+x, cur_y*16+y);
            }
        }
        cur_x++;
    }
}

void print(const char *s) {
    if(graphics_mode==0) { // 4bpp
        bios_print(s, 6);
    } else { //16bpp
        while(*s) {
            if(*s=='\n') {
                newline();
            } else {
                printchar(*s);
            }
            s++;
        }
    }
}

void __attribute__((optimize("O0"))) pic_handler() { // PIC is set to 125 Hz (previously was 1000 Hz)
	__asm("pusha");

	time_ms=time_ms+8;
	if(cursorblinktick) cursort=cursort+8;
	music_handle(); // main song
	music_handle2(); // song after main

    // cursor blinking routine
	u8 lx,ly;
	if(cursort>=500) {
		cursort=0;
		getcursor(&lx,&ly);
		if(cursorblinks==1) {
			if(cursorblinken==1) {
				setcursor(cursorblinkx,cursorblinky);
				printchar('_');
			}
			cursorblinks=0;
		} else {
			if(cursorblinken==1) {
				setcursor(cursorblinkx,cursorblinky);
				printchar(' ');
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
	set_timer_hz(125);

	ivt_set_callback(&pic_handler,8); //8-8=irq 0
}


void drawascii(const char *s,u8 color,u8 cx,u8 cy) {
	u8 dontpe=0;
	u16 tx,ty;
	u16 ttx, tty;
	int ptr=0;


	if(playmusic==0) {
		dontpe=1;
	}
	playmusic=0;

	//for(ty=0;ty<16;ty++) {
	//	for(tx=0;tx<40;tx++) { // Clear last ascii art
	//		disppixel(0,(ty+cy*16)*100+tx+cx);
	//	}
	//}

    ty=cy;
    tx=0;
    
    while(1) {
        if(s[ptr]=='\n' || s[ptr]=='\0') {
            while(tx<40) {
                // draw spaces to clear old ascii art
                if(graphics_mode==0) { // legacy
                    vga_set_plane(1);
                    for(int b=0;b<16;b++) {
			            disppixel(0,(ty*16+b)*100+cx+tx);
			        }
                    vga_set_plane(2);
                    for(int b=0;b<16;b++) {
			            disppixel(0,(ty*16+b)*100+cx+tx);
			        }
			    } else if(graphics_mode==1) {
			        for(int y=0;y<16;y++) {
		                for(int x=0;x<8;x++) {     
            		        disppixel_16bpp(0, (cx+tx)*8+x, ty*16+y);
		                }
		            }
                }
			    tx++;
            }
            if(s[ptr]=='\0') break;
            ty++;
            tx=0;
	    } else {
		    u8 charindex;
		    // looks bad, but it should be fastest
		    switch(s[ptr]) {
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
		    if(graphics_mode==0) { // legacy
                vga_set_plane(1);
                for(int b=0;b<16;b++) {
			        disppixel(ascii_charmap[b][charindex],(ty*16+b)*100+cx+tx);
		        }
		        vga_set_plane(2);
                for(int b=0;b<16;b++) {
			        disppixel(ascii_charmap[b][charindex],(ty*16+b)*100+cx+tx);
		        }
		    } else if(graphics_mode==1) { // 16bpp
		        for(int y=0;y<16;y++) {
		            u8 line = VGA_FONT[s[ptr]*16+y];
		            for(int x=0;x<8;x++) {
		                u16 color=0;
		                if((line>>(7-x)) & 1) {
                            color=get_orange_color();                
		                }
        		        disppixel_16bpp(color, (cx+tx)*8+x, ty*16+y);
		            }
		        }
		    }
		    tx++;
        }
        ptr++;
    }
    
	if(graphics_mode==0) vga_set_plane(0);
	if(dontpe==0) playmusic=1;
}

const char to_render[16] = {'.',',','-',':',';','/','H','@','M','+','X','=','#','%','$',' '}; // defining this inside prepare_ascii() causes freeze... 
	
void prepare_ascii() { // Render characters to ascii_charmap[] before demo (only 4bpp)
    if(graphics_mode==1) return;

	for(int i=0;i<16;i++) {
		vga_setcursor(0,0);
	    bios_printchar(to_render[i],15);
        for(int b=0;b<16;b++) {
             // legacy 4bpp
            ascii_charmap[b][i]=getpixel(b*100);		    
        }		
	}
    
}


void clear() {
    if(graphics_mode==0) { // legacy 4bpp
	    for(int ix=0;ix<47;ix++) {
		    for(int iy=0;iy<33*16;iy++) {
			    disppixel(0,(iy+16)*100+1+ix);
		    }
	    }    
    } else if(graphics_mode==1) { // 16bpp
        for(int ix=0;ix<47*8;ix++) {
		    for(int iy=0;iy<33*16;iy++) {
			 	disppixel_16bpp(0, 8+ix, 8+iy);
		    }
	    }    
    }
    
}

// This function is also responsible for handling events (like ascii draw)
void typeslow(const char *s, u8 time) {
	u8 cx,cy;
	u8 donewline=0;
	u8 skipfirstchar=0;
	u8 skipspace=0;

	getcursor(&cx,&cy);

	setcursor(cursorblinkx,cursorblinky);
	printchar(' ');
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
        printchar(*s);
		if(cursorblinks==0) {
			printchar('_');
			setcursor(tx+1,ty);
		}
		sleep(time);

		t_next:
        s++;
		i++;
    }
	if(donewline) {
		cy++;
		printchar(' ');
		cursorblinkx=2;
		cursorblinky=cy;
        setcursor(1,cy);
	}
	if(!skipspace) printchar(' ');
}


void main(void) {
	u8 cx,cy;
	bios_print(text_startup,7);
	newline();
	bios_print("[Test version (16/03/2024)]", 7);
	_gounreal(); 
	bios_printchar(' ',7);
	bios_printchar('.',7);
	
	__asm("cli");
	interrupt_setup();
	__asm("sti");
	sleep(3000);
	resetDisk(0); // to stop floppy from spinning?
	bios_printchar('.',7);
	
	graphics_mode = vesa_switch_to_800_600(); // graphics.c

	hide_cursor();
	
    // Get font (if 4bpp)
	prepare_ascii();

    // Draw borders
	setcursor(0,0);

	for(u8 x=0;x<50;x++) {
		printchar('-');
	}


	printchar(' ');
	for(u8 x=0;x<48;x++) {
		printchar('-');
	}

	for(u8 y=1;y<34;y++) {
		setcursor(0,y);
		printchar('|');
		setcursor(49,y);
		printchar('|');
		if(y<16) {
			setcursor(50,y);
			printchar('|');
			setcursor(99,y);
			printchar('|');
		}
	}

	setcursor(0,34);
	for(u8 x=0;x<50;x++) {
		printchar('-');
	}

	setcursor(51,15);
	for(u8 x=0;x<48;x++) {
		printchar('-');
	}
	// Start

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
	// clear entire screen
	if(graphics_mode==0) {
	    for(int x=0;x<100;x++) { // 800 / 8 = 100
	        for(int y=0;y<640;y++) {
	            disppixel(0, x+y*100);
	        }
	    }
	} else if(graphics_mode==1) {
        for(int x=0;x<800;x++) {
            for(int y=0;y<600;y++) {
               	disppixel_16bpp(0, x, y);
            }
        }
    }
	setcursor(17,17);
	print("THANK YOU FOR PARTICIPATING IN THIS ENRICHMENT CENTER ACTIVITY!!");
	musicpos=0;
	nexttick=0;
	playendmusic=1;
	while(1) {

	}
}
