// to get it running change the #include paths form "/dev/" to where you store the library
#include <stdlib.h>
#include <stdfix.h>
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
#define ENABLE_LINEDRAWING
#define ENABLE_TRIANGLES   // line drawing must be enabled for triangles to work
#define NR_LINES 0
#define NR_TRIS (6*2)
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////

#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
//#define ENABLE_LAYERS
//#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)
////////
#pragma GCC optimize ("Ofast")

#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines


#define PIXEL_CALLBACK pixelxy
#pragma GCC optimize ("Ofast")



__flash const unsigned char sinus[256] = {64,65,67,68,70,72,73,75,
	76,78,79,81,82,84,85,87,
	88,90,91,92,94,95,97,98,
	99,100,102,103,104,105,107,108,
	109,110,111,112,113,114,115,116,
	117,118,118,119,120,121,121,122,
	123,123,124,124,125,125,126,126,
	126,127,127,127,127,127,127,127,
	128,127,127,127,127,127,127,127,
	126,126,126,125,125,124,124,123,
	123,122,121,121,120,119,118,118,
	117,116,115,114,113,112,111,110,
	109,108,107,105,104,103,102,100,
	99,98,97,95,94,92,91,90,
	88,87,85,84,82,81,79,78,
	76,75,73,72,70,68,67,65,
	64,62,61,59,58,56,54,53,
	51,50,48,47,45,44,42,41,
	39,38,36,35,34,32,31,30,
	28,27,26,25,23,22,21,20,
	19,18,17,15,14,13,13,12,
	11,10,9,8,8,7,6,5,
	5,4,4,3,3,2,2,2,
	1,1,1,0,0,0,0,0,
	0,0,0,0,0,0,1,1,
	1,2,2,2,3,3,4,4,
	5,5,6,7,8,8,9,10,
	11,12,13,13,14,15,17,18,
	19,20,21,22,23,25,26,27,
	28,30,31,32,34,35,36,38,
	39,41,42,44,45,47,48,50,
51,53,54,56,58,59,61,62};


u8 ctab[64];

u16 pos[8]={0,32,64,96, 100,120,140,160};

static u8 pixelxy(u8 x, u8 y)
{
	if(y==0)
	{
		memset(ctab,0,sizeof(ctab));
		int i;
		int h=0;
		for(i=0;i<8;i++)
		{
			int v=sinus[(pos[i]+x)&0xff]>>2;
			v+=4*i;
			//	v>>=2;
			if(v>h)
			{
				int c=i*8;
				int r;
				for(r=h;r<v;r++)ctab[r]=c,c-=2;
				h=v;
			}
			int m[8]={-5,-3,-2,-1,4,2,7,8};
			if(x==127)
			pos[i]+=m[i];
		}
		
	}

	return ctab[y];
}

#include "/dev/displayfunc.h"


extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	for (;;)
	{
		Display(0, 0, 0,0);
	}
}
