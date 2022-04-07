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
#define SECOND_CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define ENABLE_SECOND_CONSOLE

#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define SECOND_CONSOLE_LINE_START (0*8)

////////
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
/////////////////////////////////////////////////////////////////////

__flash const u8 diagram_overlay[] =
{
	#include "/dev/alien.raw.h"
};


unsigned char counter=0;

s8 table[64];
unsigned char cnt=0;

extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static unsigned char console[16*8];
	static unsigned char console2[16*8];
	SystemServer_WriteToScreen("we come in peace",console,0,2);
	SystemServer_WriteToScreen("we bring cookies",console,0,3);
	for(int i=0;i<8;i++)
		SystemServer_WriteToScreen("hello",console2,i,i);
	for(;;)
	{		
		for(u16 zoom=64;zoom<512;zoom+=3)
			Display(0, 0, 0,console,console2,zoom,zoom);
		for(u16 zoom=64;zoom<512;zoom+=3)
			Display(0, 0, 0,console,console2,256,zoom);
		for(u16 zoom=64;zoom<512;zoom+=3)
			Display(0, 0, 0,console,console2,zoom,256);
	}
}


