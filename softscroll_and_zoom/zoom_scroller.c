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
unsigned char calc_alien_bg(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_END (SECOND_CONSOLE_LINE_START+(SECOND_CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_LENGTH 17
#define SECOND_CONSOLE_LINE_ENABE_SCROLL
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define ENABLE_SECOND_CONSOLE

#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define SECOND_CONSOLE_LINE_START (0*8)

////////
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////


unsigned char counter=0;

s8 table[64];
unsigned char cnt=0;

extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	static unsigned char console[1*8];
	static unsigned char console2[SECOND_CONSOLE_LINE_LENGTH*8];
	SystemServer_WriteToScreen("we come in peace",console,0,0);
int scrollX=0;
int scrollY=0;
u16 zoom=256;

	for(;;)
	{	
		const char s[]="                the quick brown fox jumps over the lazy dog                ";
		if(scrollX>(75-17)*8)
		{
			scrollX=0;
			zoom-=64;
			if(zoom<32)zoom=256;	
		}
		else scrollX++;
		
		int p=scrollX>>3;
		for(char i=0;i<17;i++)console2[i+17]=ApiCharToFontIndex(s[i+p]);		
		Display(0, 0, 0,console,console2,zoom,zoom,(scrollX&7)*256,0);
	}
}


