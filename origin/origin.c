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

//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384
#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
unsigned char calc_origin_effect(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_origin_effect
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)
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


unsigned char counter=0;

static u8 table[64];
unsigned char cnt=0;
unsigned char calc_origin_effect(unsigned char x,unsigned char y)
{
	if(y==0)
	{
		table[((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)&0x3f]=63;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)-1)&0x3f]=40;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)-2)&0x3f]=30;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)+1)&0x3f]=40;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)+2)&0x3f]=30;

		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)-1)&0x3f]=20;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)-2)&0x3f]=10;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)+1)&0x3f]=20;
		table[(((sin_tab[((x+8*cnt)&0xff)]+sin_tab[((x*3+7*cnt)&0xff)])>>1)+2)&0x3f]=10;


		cnt++;
	}
	if(!(x&7))
	if (table[y])
	{
		table[y]--;
	}
	return table[y];
}
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	unsigned char console[16*1];
	memset(console,0,sizeof(console));
	unsigned char flip=0;
	for (;;)
	{	
		if((counter&0x3f)==0)
		{ 
			SystemServer_WriteToScreen("here come from",console,0,0);
			if(flip)
			SystemServer_WriteToScreen("we outer space",console,0,0);
			flip^=1;
		}
		cnt=counter;
		Display(0, 0, 0,console);
		memset(table,0,64);
		counter++;
	}
}


