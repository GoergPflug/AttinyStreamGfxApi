#define ENABLE_TRIANGLES
//#define ENABLE_ARDUINO_SUPPORT
//#define ENABLE_WIRE
#define WIRE_SCREEN_ADDRESS 0x3C
#define ENABLE_LINEDRAWING
#define NR_LINES 12
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384


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

//#define ENABLE_USERFONT 1
#define NR_CIRCLES 16
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK render_lines
//#define ENABLE_LAYERS
//#define ENABLE_CONSOLE
#define NR_LAYERS 0
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)
////////
#define ENABLE_CIRCLES
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
/////////////////////////////////////////////////////////////////////
// this is NOT how you should design a 3D-Engine, one should use matrix multiplications, but it demonstrates how to draw lines


#pragma GCC optimize ("Ofast")


void MainTask() {
	// put your main code here, to run repeatedly:
	u8 inverted=0;
	GfxApiSetBrightness(15);
	int fr=15;
	float refr_wobble=0,refr_wobble_cnt=0,refr_wobble_mul=1;
	for (;;)
	{
		if(rand()<RAND_MAX/80)inverted^=1,refr_wobble=1,refr_wobble_cnt=0,refr_wobble_mul=7,fr=0;
		GfxApiSetInvert(inverted);
		GfxApiSetFreq(fr);
		if(fr<15)fr++;

		refr_wobble_cnt+=0.5;
		//GfxApiSetBrightness(br);
		if(refr_wobble_mul>0)refr_wobble_mul-=0.025;else refr_wobble_mul=0;
		GfxApiBeginCircles();
		int i;
		static float c;
		c+=0.05;

		const u8 p[]={10,20,30,40};
		
		for(i=0;i<16;i++)
		{
			GfxApiStoreCircle(128+32*sin((i*c)/10.0+c),128+31*sin((i*c)/11.0+c),sin((i+c)/12+c)*10+10,sin((i+c)/13+c)*31+31);
		}
		Display(0, 0, 0,0);
	}
}