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
unsigned char calc_face(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_face
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

__flash const unsigned char face_flash[]=
{
	0,0,6,10,12,13,14,16,17,17,18,19,20,21,22,22,23,23,24,24,24,25,25,25,25,25,25,25,25,25,26,26,27,28,29,30,31,32,32,31,30,29,28,28,29,28,26,28,28,27,25,25,25,26,25,25,24,20,10,9,9,9,9,
};
static u8 face[64];

unsigned char calc_face(unsigned char x,unsigned char y)
{
	if(x>64)
	{
		if(127-face[y]>x)return 0;
		return x/2;
	}
	if(face[y]<x)return 63;
	return x/2;
	
}
extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	float cnt=0;
	float dir=0.05;
	for (;;)
	{
		cnt+=dir;
		if(cnt>1.5)dir=-dir;
		if(cnt<0)dir=-dir,cnt=0;
		u8 i;
		for(i=0;i<64;i++)face[i]=face_flash[i]*cnt;
		Display(0, 0, 0,0);
	}
}


