// IVT.C 
/*
	ivt/ivt related functions
	
	Written by maniek86 2022 (c) 
*/

char *ivt = (u8*)0x0000; 

void ivt_set_callback(void *function, u8 i) {
    u32 pointer = (void *)function;
	ivt[i*4] = (u8)(pointer & 0xFF);
	ivt[i*4+1] = (u8)((pointer >> 8) & 0xFF);
	ivt[i*4+2] = (u8)((pointer >> 16) & 0xFF);
	ivt[i*4+3] = (u8)((pointer >> 24) & 0xFF);
}

void set_timer_hz(u32 hz) {
	u32 divisor = 1193180 / hz;
	outb(0x43, 0x36);
	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);
	outb(0x40, l);
	outb(0x40, h);
}
