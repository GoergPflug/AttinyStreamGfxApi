#include <Wire.h>


#define ENABLE_ARDUINO_SUPPORT
#define ENABLE_WIRE
#define WIRE_SCREEN_ADDRESS 0x3C
//#define ENABLE_LINEDRAWING
//#define NR_LINES 12
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384

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
#define REPLACE_FONT "bad_apple_full_charset.h"
#define ENABLE_CONSOLE0_FONT_SWITCHING



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
#include "tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////

//const u8 os_font[] __attribute__((progmem)) = {
  
#include "full_apple_vid.h"

int counter=0;

void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}

void loop() 
{
	
long long chunks[]
{
	pgm_get_far_address(chunk_0),
	pgm_get_far_address(chunk_1),
	pgm_get_far_address(chunk_2),
	pgm_get_far_address(chunk_3),
	pgm_get_far_address(chunk_4),
	pgm_get_far_address(chunk_5),
	pgm_get_far_address(chunk_6),
	pgm_get_far_address(chunk_7),	
	pgm_get_far_address(chunk_8),
	pgm_get_far_address(chunk_9),
	pgm_get_far_address(chunk_10),
	pgm_get_far_address(chunk_11),
	pgm_get_far_address(chunk_12),
	pgm_get_far_address(chunk_13),
	pgm_get_far_address(chunk_14),
	pgm_get_far_address(chunk_15),
	pgm_get_far_address(chunk_16),
	pgm_get_far_address(chunk_17),		

};

	int dir=1;
	int s_pos=0;
	static unsigned char console[16*8];
	
	long long chunk=0;
  
	for(;;)
	{

		int frame;
		for (frame=0;frame<200;frame++)
		{
			int i;
			for(i=0;i<128;)
			{
				long long addr2=chunks[chunk];
				addr2+=s_pos;
				int v=pgm_read_byte_far(addr2);
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
			long long addr = chunk;
			addr*=1024;
			Display(0, 0, 0,console,addr);
	//		Display(0, 0, 0,console,chunk*1024);
		//	Display(0, 0, 0,console,chunk*1024);
			
		//	if(s_pos>=sizeof(chunk_0))s_pos=0;
		}
		chunk++;
		s_pos=0;
		if(chunk==17)chunk=0;
	}
}


