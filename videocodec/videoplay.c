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
#define REPLACE_FONT "charset.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
//#define ENABLE_LAYERS

//#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define ENABLE_CONSOLE
#define NR_LAYERS 0
#define NR_SPRITES 0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
#define DISABLE_OFAST
////////
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
/////////////////////////////////////////////////////////////////////


const __flash u8 sequence[]=
{
	#include "vid.h"
};

int counter=0;

extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	int dir=1;
	int s_pos=0;
	static unsigned char console[16*8];
	for (;;)
	{
		int i;
		for(i=0;i<128;)
		{
			int v=sequence[s_pos];
			if(v>127)
			{
				i+=v-127;
				s_pos++;
			}
			else
			{
				console[i]=v;
				i++;
				s_pos++;
			}
		}
		Display(0, 0, 0,console);
		Display(0, 0, 0,console);
		Display(0, 0, 0,console);
		Display(0, 0, 0,console);
		Display(0, 0, 0,console);
		Display(0, 0, 0,console);
		
		if(s_pos>=sizeof(sequence))s_pos=0;
	}
}


