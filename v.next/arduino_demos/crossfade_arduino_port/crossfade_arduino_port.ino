// to get it working point the #includes from "/dev/...." to the place where you store tiny_multi_os.h and displayfunc.h

#include <Wire.h>


#define ENABLE_ARDUINO_SUPPORT
//#define ENABLE_WIRE
#define WIRE_SCREEN_ADDRESS 0x3C


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
static unsigned char calc_crossfade(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_crossfade
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

 const u8 face_overlay2[] PROGMEM=
{
	#include "img1.h"
};

 const u8 face_overlay[] PROGMEM =
{
	#include "img2.h"
};

unsigned char counter=0;

volatile u8 table[256];
unsigned char cnt=0;
static GfxApiCompressedLayer Overlay;

static GfxApiCompressedLayer Overlay2;

static unsigned char calc_crossfade(unsigned char x,unsigned char y)
{
	GfxApiLayerGetNextByte(&Overlay);
	GfxApiLayerGetNextByte(&Overlay2);
	return table[(Overlay2.PixelValue>>2)|((Overlay.PixelValue<<2)&0xf0)];//+table[0];
}


void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}


void loop()
{
	
	float mix=0;
	float mix_delta=0.1;
	asm volatile("nop");
	
	for (;;)
	{
		mix=mix+mix_delta;
		if(mix>2.0f)mix=2,mix_delta=-0.05f;
		if(mix<0.0f)mix=0,mix_delta=0.05f;
		
		float mix2=mix-0.5f;
		if(mix2<0)mix2=0;
		if(mix2>1)mix2=1;
		int i;
		for(i=0;i<256;i++)
		{
			float a=(i&0xf);
			float b=(i/16);
			float mixed=a*mix2+b*(1.0-mix2);
			int c=mixed*4.0f;
			table[i]=c;
		}
		
		Overlay.Bitpos=1;
		Overlay.BytePos=&face_overlay[0];
		Overlay.PixelValue=0;
		Overlay.SkipCounter=0;
		
		Overlay2.Bitpos=1;
		Overlay2.BytePos=&face_overlay2[0];
		Overlay2.PixelValue=0;
		Overlay2.SkipCounter=0;

		cnt++;

		Display(0, 0, 0,0);
	}
}
