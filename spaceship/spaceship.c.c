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
unsigned char calc_alien_bg(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_alien_bg
#define ENABLE_LAYERS
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
__flash const u8 spaceship_overlay[] =
{
	#include "/dev/spaceship2.raw.h"
};

unsigned char counter=0;

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
	return b<<2;
}



unsigned char calc_alien_bg(unsigned char x,unsigned char y)
{
	return rnd();
}
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static GfxApiCompressedLayer Overlay;
	counter++;
	os_gfx_start_display_transfer();
	
	char fade=0;
	for (;;)
	{
		Overlay.Bitpos = 1;
		Overlay.BytePos = &spaceship_overlay[0];
		Overlay.SkipCounter = 0;
		Overlay.PixelValue = 0;
		fade++;
		Display(&Overlay, fade&0x7, 0,0);
		counter++;
	}
}


