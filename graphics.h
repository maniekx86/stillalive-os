#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

void bios_printchar(char chr, u8 color);
void bios_print(const char *s,u8 color);
void setchar(u8 x,u8 y,u8 c,u8 color);
void vga_setcursor(u8 x, u8 y);
void vga_getcursor(u8 *x, u8 *y);
void vga_newline(void);
void setvideomode(u8 mode);
void vga_set_plane(uint8_t p);
void vesa_get_mode_inf();
void vga_switch_font();
u8 vga_getmode();
u8 vesa_switch_to_800_600();
void vesa_set_mode(u16 mode);
void disppixel_16bpp(u16 data, u16 x, u16 y);
u16 getpixel_16bpp(u16 x, u16 y);
u16 get_orange_color();


// VESA

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

struct VbeInfoBlock {
   char VbeSignature[4];             // == "VESA"
   uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
   uint16_t OemStringPtr[2];            // isa vbeFarPtr
   uint8_t Capabilities[4];
   uint16_t VideoModePtr[2];         // isa vbeFarPtr
   uint16_t TotalMemory;             // as # of 64KB blocks
   uint8_t reserved1[237];
} __attribute__ ((packed));


#endif //__GRAPHICS_H__
