#include "typedef.h"
#include "misc.h"
#include "data.h"
#include "music.h"

extern u8 playmusic;
extern u8 playendmusic;

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

extern u16 nexttick;
extern int32_t musicpos;
extern u8 theend;

extern int textn;
extern int textndelay;
extern int currenttext;

void music_handle() {
	if(playmusic == 0) return;
	nexttick++;
	if(nexttick < 63) return; // 120 bpm
	nexttick = 0;
	if(musicdata[musicpos] == 0) {
		if(musicdata[musicpos + 1] != 0) {
			nosound();
		}
	} else if(musicdata[musicpos] == 255) {
		nosound();
		outb(0x80,0);
	} else if(musicdata[musicpos] == 254) {
		nosound();
		outb(0x80,255);
		playmusic = 0;
	} else {
		if(musicdata[musicpos] < 100) play_sound(NOTEFREQ[musicdata[musicpos]]);
		outb(0x80,musicdata[musicpos]);
	}
	if(theend==0&&lyricsync[musicpos] != 0) {
		if(textn != -1) {
			qemu_debugcon("WORD SKIPPED I HATE MY LIFE");
		}
		if(lyricsync[musicpos] == 255) {
			theend = 1;
		}
		textn=currenttext;
		textndelay=lyricsync[musicpos];
		currenttext++;
	}
	musicpos++;
}

void music_handle2() {
	if(playendmusic == 0) return;
	nexttick++;
	if(nexttick < 63) return; // 120 bpm
	nexttick = 0;
	if(musicdata2[musicpos] == 0) {
		if(musicdata2[musicpos+1] != 0) {
			nosound();
		}
	} else if(musicdata2[musicpos] == 255) {
		nosound();
		outb(0x80, 255);
	} else if(musicdata2[musicpos] == 254) {
		musicpos = -1;
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
