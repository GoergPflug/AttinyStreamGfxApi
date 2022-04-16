#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
// OS-Configuration////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define ENABLE_CONSOLE
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384
//#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
unsigned char calc_bg(unsigned char x,unsigned char y);
#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_bg
//#define ENABLE_LAYERS

#define NR_LAYERS 0
#define NR_SPRITES 1
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

////////
#include "tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////
__flash const u8 sin_tab[256] =
{
	32, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 61, 61, 61, 60, 60, 60, 59, 59, 59, 58, 58, 57, 57, 56, 56, 55, 55, 54, 54, 53, 52, 52, 51, 51, 50, 49, 49, 48, 47, 47, 46, 45, 44, 44, 43, 42, 42, 41, 40, 39, 39, 38, 37, 36, 35, 35, 34, 33, 32, 31, 31, 30, 29, 28, 28, 27, 26, 25, 24, 24, 23, 22, 21, 21, 20, 19, 19, 18, 17, 16, 16, 15, 14, 14, 13, 12, 12, 11, 11, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31
};

__flash const u8 mauszeiger[]=
{
	0b11111111,
	0b11111110,
	0b11111100,
	0b11111000,
	0b11110000,
	0b11100000,
	0b11000000,
	0b10000000,
	
};

static unsigned char cnt=0;

unsigned char rnd()
{
	static char tab[16] = { 0,1,2,3 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	static char a,b,c;
	a++;
	a &= 0x0f;
	c = tab[b];
	int tmp = tab[a];
	tab[a] = tab[b];
	tab[b] = tmp;
	b = c;
	return b;
}
unsigned char calc_bg(unsigned char x,unsigned char y)
{
	unsigned char col=0;
	if((y>3*8+4)||(x>90))
	if((!((x)&7))||(!(((y-2))&7)))col=63;
	
	u8 y_wave=15+((sin_tab[(cnt+x*3)&0xff]+((x*3+2*cnt)&0xf))>>1);
	s8 dist=(y-y_wave);
	if ((dist>0)&&(dist<6))
	{
		s8 dist2=abs((int)x-(int)cnt/2);
		col=63;
		if (dist2<16)col=63-((15-dist2)*4);
		
	}
	return col;
}
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static char console[16*8];
	int cnt2=0;
	GfxApiSprite maus;
	
	int cnt3=100;
	char mode=0;
	for (;;)
	{
		cnt3--;
		if(!cnt3)cnt3=100,mode^=1;
		maus.readpos_bit=1;
		maus.readpos_byte=&mauszeiger[0];
		maus.sprite_height=sizeof(mauszeiger);
		maus.SkipCounter=16+(sin_tab[(cnt2*3)&0xff]>>1)+64*(16+sin_tab[(cnt2)&0xff]);
		if(mode)maus.SkipCounter=0xffff;
		cnt2++;
		memset(console,0,sizeof(console));
		if (mode==0)
		{
			if((cnt>>5)&1)
			SystemServer_WriteToScreen("Signal Lock",console,0,0);
			else
			SystemServer_WriteToScreen("Scanning   ",console,0,0);
			

			SystemServer_WriteToScreen("Alpha",console,0,1);
			SystemServer_WriteToScreen("Delta",console,0,2);
			
			console[16+8]=1+(cnt&0xf);
			console[16+9]=1+(cnt>>4);
			
			console[16*2+8]=1+((cnt+55)&0xf);
			console[16*2+9]=1+((cnt>>3)&0xf);

		}
		else
		{
			SystemServer_WriteToScreen("OGX3500M",console,0,0);
			SystemServer_WriteToScreen("Messapparat",console,0,1);
		}
		Display(0, 0, &maus,console);
		cnt++;
	}
}


