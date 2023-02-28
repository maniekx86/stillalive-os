#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

void bios_printchar(char chr, u8 color);
void bios_print(const char *s,u8 color);
void setchar(u8 x,u8 y,u8 c,u8 color);
void setcursor(u8 x, u8 y);
void getcursor(u8 *x, u8 *y);
void newline(void);
void setvideomode(u8 mode);
#endif //__GRAPHICS_H__
