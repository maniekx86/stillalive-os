#ifndef __MEMACCESS_H__
#define __MEMACCESS_H__

#include "stdint.h"

#define fastcall __attribute__((regparm(3)))
#define asmlinkage __attribute__((regparm(0)))

void set_video_seg(int seg);

/* Display character with FS change */
fastcall void dispchar(uint16_t celldata, uint16_t offset);

fastcall void disppixel(uint8_t celldata, uint16_t offset);

fastcall uint16_t getchar(uint16_t offset);

fastcall uint8_t getpixel(uint16_t offset);

/* Display character with no FS change */
fastcall void dispchar_nofsupd(uint16_t celldata, uint16_t offset);

fastcall void disppixel_nofsupd(uint8_t celldata, uint16_t offset);

/* Set FS segment and return previous value */
fastcall uint32_t getset_fs(uint32_t segment);

/* Set FS segment */
fastcall void set_fs(uint32_t segment);

/* Set FS to video mode segment 0xb800 */
fastcall uint32_t set_videomode_fs(void);


static inline uint16_t chr(uint8_t ochar, uint8_t attr)
{
    return (uint16_t) (attr << 8) | (uint8_t) ochar;
}

#endif //__MEMACCESS_H__
