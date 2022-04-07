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
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)
////////
#include "tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////

__flash const u8 flexgrid_overlay[] =
{
	#include "flexgrid.raw.h"
};

unsigned char counter=0;
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static GfxApiCompressedLayer Overlay;
	counter++;
	os_gfx_start_display_transfer();

	static unsigned char console[16*8];
	for (;;)
	{
			int x,y;
			for(y=0;y<8;y++)
			for(x=0;x<16;x++)
				console[x+16*y]=((x+y+counter)&7)+37;
				Overlay.Bitpos = 1;
				Overlay.BytePos = &flexgrid_overlay[0];
				Overlay.SkipCounter = 0;
				Overlay.PixelValue = 0;


		Display(&Overlay, 0, 0,console);
		counter++;
	}
}


