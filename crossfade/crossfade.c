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
//#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
static unsigned char calc_wobble(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_wobble
//#define ENABLE_LAYERS
//#define ENABLE_CONSOLE
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

__flash const u8 face_overlay2[] =
{
	#include "face1.raw.h"
};

__flash const u8 face_overlay[] =
{
	#include "face2.raw.h"
};

unsigned char counter=0;

volatile u8 table[256];
unsigned char cnt=0;
static GfxApiCompressedLayer Overlay;

static GfxApiCompressedLayer Overlay2;
static unsigned char calc_wobble(unsigned char x,unsigned char y)
{
	Overlay.PixelValue += GfxApiLayerGetNextByte(&Overlay);
	Overlay2.PixelValue += GfxApiLayerGetNextByte(&Overlay2);
	return table[(Overlay2.PixelValue>>2)|((Overlay.PixelValue<<2)&0xf0)];//+table[0];
}


extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	
	float mix=0;
	float mix_delta=0.1;
	asm volatile("nop");
	
	for (;;)
	{
		mix=mix+mix_delta;
		if(mix>2.0f)mix=2,mix_delta=-0.05f;
		if(mix<0.0f)mix=0,mix_delta=0.05f;
		
		float mix2=mix-0.5f;
		if(mix2<0)mix2=0;
		if(mix2>1)mix2=1;
		int i;
		for(i=0;i<256;i++)
		{
			float a=(i&0xf);
			float b=(i/16);
			float mixed=a*mix2+b*(1.0-mix2);
			int c=mixed*4.0f;
			table[i]=c;
		}
		
		Overlay.Bitpos=1;
		Overlay.BytePos=&face_overlay[0];
		Overlay.PixelValue=0;
		Overlay.SkipCounter=0;
		
		Overlay2.Bitpos=1;
		Overlay2.BytePos=&face_overlay2[0];
		Overlay2.PixelValue=0;
		Overlay2.SkipCounter=0;

		cnt++;

		Display(0, 0, 0,0);
	}
}
