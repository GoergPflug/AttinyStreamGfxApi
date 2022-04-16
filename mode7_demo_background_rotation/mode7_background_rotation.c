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

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
unsigned char calc_alien_bg(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_END (SECOND_CONSOLE_LINE_START+(SECOND_CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_LENGTH 16
#define SECOND_CONSOLE_LINE_ENABE_SCROLL
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
//#define ENABLE_LAYERS
//#define ENABLE_CONSOLE
#define ENABLE_SECOND_CONSOLE

#define NR_LAYERS 0
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define SECOND_CONSOLE_LINE_START (0*8)
#define SECOND_CONSOLE_AFFINE_CALLBACK 1
////////
#define ENABLE_FONT_BASIC

#define ENABLE_SECOND_CONSOLE_AFFINE

#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
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
	static unsigned char console[16*8];
	SystemServer_WriteToScreen("we come in peace",console,0,0);
	int scrollX=0;
	int scrollY=0;
	
	float rot=0;
	
	s16 matrix[4];
	
	
	int i;for(i=0x20;i<128;i++)console[i-0x20+16]=i-0x20;
	for(;;)
	{
		matrix[0]=256.0*cos(rot);
		matrix[1]=256.0*sin(rot);
		matrix[2]=-256.0*sin(rot);
		matrix[3]=256.0*cos(rot);
		rot+=0.03;
		
		static	const __flash char s[]  ="the quick brown fox jumps over the lazy dog                ";
		if(scrollX>(75-17)*8)
		{
			scrollX=0;
		//	zoom-=64;
			//if(zoom<32)zoom=256;
		}
		else scrollX++;
		
		int p=0;//scrollX>>3;
		Display(0, 0, 0,0,console,matrix[0],matrix[3],scrollX*256,scrollX*256,matrix[1],matrix[2]);
	}
}


