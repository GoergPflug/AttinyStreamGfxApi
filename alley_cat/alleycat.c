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
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
#define ENABLE_LAYERS

#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
#define CONSOLE_SIZE_Y 4
#define CONSOLE_LINE_START (2*8)
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

////////
#include "tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////

__flash const u8 cat_overlay[] =
{
	#include "cat1.raw.h"
};

__flash const u8 cat_overlay2[] =
{
	#include "cat2.raw.h"
};

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


unsigned char counter=0;
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static GfxApiCompressedLayer Overlay;
	counter++;
	static unsigned char console[16*4];
	for (;;)
	{
		char i;
		if(counter&0x40)
		{
			for(i=0;i<16*2;i++)console[i]=rnd()+1;
			for(i=16*2+8;i<16*3;i++)console[i]=rnd()+1;
		}
		if(!counter)for(i=0;i<16*2;i++)console[i]=0;

		char t=(counter>>4)&1;
		Overlay.Bitpos = 1;
		Overlay.BytePos = &cat_overlay[0];
		if (t)		Overlay.BytePos = &cat_overlay2[0];
		Overlay.SkipCounter = 15*64;
		Overlay.PixelValue = 0;
		SystemServer_WriteToScreen("Alley Cat ",console,0,3);
		Display(&Overlay, 0, 0,console);
		counter++;
	}
}


