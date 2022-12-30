// memaccess.c
/*
	File for accessing graphics memory in real mode

	Not written by maniek86 2022 (c) - https://stackoverflow.com/questions/37354717/displaying-text-video-memory-at-0xb8000-without-using-the-c-library
*/


#define fastcall __attribute__((regparm(3)))
#define asmlinkage __attribute__((regparm(0)))

extern fastcall void dispchar(uint16_t celldata, uint16_t offset);
extern fastcall uint16_t getchar(uint16_t offset);

extern fastcall uint32_t getset_fs(uint32_t segment);
extern fastcall void set_fs(uint32_t segment);
extern fastcall uint32_t set_videomode_fs(void);
static inline uint16_t tm_charattr_to_celldata(uint8_t ochar, uint8_t attr);

int VIDEO_SEG=0xa000;

void set_video_seg(int seg) {
    VIDEO_SEG=seg;
}

static inline uint16_t chr(uint8_t ochar, uint8_t attr)
{
    return (uint16_t) (attr << 8) | (uint8_t) ochar;
}


/* Display character with FS change */
fastcall void dispchar(uint16_t celldata, uint16_t offset)
{
    uint32_t oldfs = set_videomode_fs();
    dispchar_nofsupd(celldata, offset);
    set_fs(oldfs);
}

fastcall void disppixel(uint8_t celldata, uint16_t offset)
{
    uint32_t oldfs = set_videomode_fs();
    disppixel_nofsupd(celldata, offset);
    set_fs(oldfs);
}

fastcall uint16_t getchar(uint16_t offset)
{
    uint16_t ret=0;
    uint32_t oldfs = set_videomode_fs();
    __asm__ __volatile__("movw %%fs:%[memloc], %w[ret]\n\t"
                         :[ret] "=&rm"(ret)
                         :[memloc] "m"(*(uint32_t *)(uint32_t)offset));
    set_fs(oldfs);
    return ret;
}

fastcall uint8_t getpixel(uint16_t offset)
{
    uint8_t ret=0;
    uint32_t oldfs = set_videomode_fs();
    __asm__ __volatile__("movb %%fs:%[memloc], %b[ret]\n\t"
                         :[ret] "=&rm"(ret)
                         :[memloc] "m"(*(uint32_t *)(uint32_t)offset));
    set_fs(oldfs);
    return ret;
}

/* Display character with no FS change */
fastcall void dispchar_nofsupd(uint16_t celldata, uint16_t offset)
{
    __asm__ ("movw %w[wordval], %%fs:%[memloc]\n\t"
             :
             :[wordval]"ri"(celldata),
              [memloc] "m"(*(uint32_t *)(uint32_t)offset)
              :"memory");
}

fastcall void disppixel_nofsupd(uint8_t celldata, uint16_t offset)
{
    __asm__ ("movb %b[wordval], %%fs:%[memloc]\n\t"
             :
             :[wordval]"ri"(celldata),
              [memloc] "m"(*(uint32_t *)(uint32_t)offset)
              :"memory");
}


/* Set FS segment and return previous value */
fastcall uint32_t getset_fs(uint32_t segment)
{
    uint32_t origfs;
    __asm__ __volatile__("mov %%fs, %w[origfs]\n\t"
                         "mov %w[segment], %%fs\n\t"
                         :[origfs] "=&rm"(origfs)
                         :[segment] "rm"(segment));
    return origfs;
}

/* Set FS segment */
fastcall void set_fs(uint32_t segment)
{
    __asm__("mov %w[segment], %%fs\n\t"
            :
            :[segment]"rm"(segment));
}

/* Set FS to video mode segment 0xb800 */
fastcall uint32_t set_videomode_fs(void)
{
    return getset_fs(VIDEO_SEG);
}


