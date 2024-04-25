// graphics.c
/*
	Graphics related functions

	Written by maniek86 2024 (c) 
*/

#include "memaccess.h"
#include "typedef.h"
#include "graphics.h"
#include "misc.h"
#include "data.h"

extern void _setmem(u32, u8);
extern void _setmemw(u32, u16);
extern uint8_t _getmem(u32);
extern uint16_t _getmemw(u32);

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
            vga_newline();
        } else {
            bios_printchar(*s,color);
        }
        s++;
    }
}


void setchar(u8 x,u8 y,u8 c,u8 color) {
    dispchar((color << 8) | c,(y*80+x)*2);
}

void vga_setcursor(u8 x, u8 y) {
    __asm__(
        "int $0x10"
        :
        : "a"((0x02 << 8) | 0), "d"((y << 8) | x), "b"(0));
}

void vga_getcursor(u8 *x, u8 *y) {
    uint16_t out;
    __asm__(
        "int $0x10"
        : "=d"(out)
        : "a"((0x03 << 8) | 0), "b"(0));
    *x = (out >> 0) & 0xFF;
    *y = (out >> 8) & 0xFF;
}


void vga_newline() {
    u8 x,y;
	vga_getcursor(&x,&y);
	if(y>23) {
        u32 i=0;
        bios_printchar('\n',0);
		vga_setcursor(0,23);
	} else {
        vga_setcursor(0,y+1);
    }
}

void setvideomode(u8 mode) {
    __asm__(
        "int $0x10"
        :
        : "a"((0x00 << 8) | mode));
}

void vga_set_plane(uint8_t p) {
    p &= 3;
    uint8_t pmask = 1 << p;
    outb(0x3CE, 4);
    outb(0x3CF, p);
    outb(0x3C4, 2);
    outb(0x3C5, pmask);
}

void vesa_get_mode_info(u16 mode, struct vbe_mode_info_structure *data) {
    __asm__ volatile(
        "mov %w[segment], %%edi;"
        "int $0x10"
        : 
        : "a"(0x4f01), "c"(mode), [segment]"rm"((uint32_t*)data)
        : "edi");
}

u8 vga_getmode() {
    uint16_t out;
    __asm__(
        "int $0x10"
        : "=a"(out)
        : "a"((0x0F << 8) | 0), "b"(0));
    return (out >> 0) & 0xFF;
}

void vga_switch_font() {
    __asm__( // this switches font to 8x16 one
        	"int $0x10"
        	:
        	: "a"(0x1124), "b"(03)); // http://www.techhelpmanual.com/165-int_10h_1124h__setup_rom_8x16_font_for_graphics_mode.html
    return;
}

void vesa_set_mode(u16 mode) {
    
    __asm__(
            "int $0x10"
            :
            : "a"(0x4f02), "b"(mode));
}



// This will:
// return 1 where VESA framebuffer if 800x600x16bpp mode is used
// or return 0 if 800x600x4bpp mode is used

/*
    ModeAttributes field:
    D0 = Mode supported by hardware configuration
        0 = Mode not supported in hardware
        1 = Mode supported in hardware
    D1 = 1 (Reserved)
    D2 = TTY Output functions supported by BIOS
        0 = TTY Output functions not supported by BIOS
        1 = TTY Output functions supported by BIOS
    D3 = Monochrome/color mode (see note below)
        0 = Monochrome mode
        1 = Color mode
    D4 = Mode type
        0 = Text mode
        1 = Graphics mode
    D5 = VGA compatible mode
        0 = Yes
        1 = No
    D6 = VGA compatible windowed memory mode is available
        0 = Yes
        1 = No
    D7 = Linear frame buffer mode is available
        0 = No
        1 = Yes
*/

static u32 fb_address;
static u16 fb_bytes_per_line;
static u8 fb_bytes_per_pixel;

u16 orange_color_16bpp;

u8 __attribute__((optimize("O0"))) vesa_switch_to_800_600() {

    struct VbeInfoBlock inf;
	struct vbe_mode_info_structure fbinfo;

	uint32_t vbe_info_pointer=(uint32_t*)&inf;

	__asm__ volatile(
        "mov %w[segment], %%edi;"
        "int $0x10"
        : 
        : "a"(0x4f00), [segment]"rm"(vbe_info_pointer)
        : "edi");
        
    // If typical VESA is not supported we can try int 0x10,0 with 0x6A
	if(!(inf.VbeSignature[0]=='V' && inf.VbeSignature[1]=='E' && inf.VbeSignature[2]=='S' && inf.VbeSignature[3]=='A')) {
    	vga_newline();
		bios_print("Warning: no VESA detected, trying anyway using 4bpp",7);
		sleep(2000);
		setvideomode(0x6A);
		vga_switch_font();
		return 0;
	}
	
	// We need to use 0x6A mode as detecting using non 0x6A (0x100>) modes disables us use of bios printing routines
	setvideomode(0x6A);
    u8 vga_mode = vga_getmode();
    
    if(vga_mode==0x6A) { // VESA 4bpp successed 
        vga_switch_font();
        return 0;
    }
	
    // We need to find 800x600x16
    u16 found_mode_16bpp=0;
    
    for(int i=0;_getmemw(inf.VideoModePtr[0]+(0x10*inf.VideoModePtr[1])+i)!=0xFFFF;) {
		u16 mode=_getmemw(inf.VideoModePtr[0]+(0x10*inf.VideoModePtr[1])+i);

		vesa_get_mode_info(mode,&fbinfo);

	    if(fbinfo.width==800&&fbinfo.height==600) {
	        if(fbinfo.bpp==16) { // Why not 8bpp? because why not 16bpp
	            found_mode_16bpp = mode; 
	            fb_address = fbinfo.framebuffer;
	            fb_bytes_per_line = fbinfo.pitch;
	            fb_bytes_per_pixel = fbinfo.bpp/8;
	            break;
	        }
		}

		i=i+2;
	}
	

	if(found_mode_16bpp!=0) {

	    // this code should guarantee comaptibility if some funny graphic card doesn't have standard 16bpp color encoding
	    u8 red = 0xAA >> (8 - fbinfo.red_mask); 
        u8 green = 0x55 >> (8 - fbinfo.green_mask); 
        u8 blue = 0x00 >> (8 - fbinfo.blue_mask);

        orange_color_16bpp = (red << fbinfo.red_position) | (green << fbinfo.green_position) | (blue << fbinfo.blue_position);

	    
	    vesa_set_mode(found_mode_16bpp); 
		return 1;
	}
	
    // no mode found for this demo
    vga_newline();
    bios_print("No suitable video mode found for this system!", 7);
	__asm("cli");
	__asm("hlt");
	while(1) {} // halt
	return 0;
}


void disppixel_16bpp(u16 data, u16 x, u16 y) {
    u32 addr = y * fb_bytes_per_line + x * fb_bytes_per_pixel;
    
    _setmemw(fb_address + addr, data);
}
u16 get_orange_color() { // get the proper way the u16 value color for 16bpp
    return orange_color_16bpp;
}
u16 getpixel_16bpp(u16 x, u16 y) {
    u32 addr = y * fb_bytes_per_line + x * fb_bytes_per_pixel;
    return _getmemw(fb_address + addr);
}

