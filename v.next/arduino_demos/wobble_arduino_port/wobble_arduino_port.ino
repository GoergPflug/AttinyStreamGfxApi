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

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
unsigned char calc_wobble(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_wobble
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
const u8 sin_tab[256] PROGMEM=
{
	32, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 61, 61, 61, 60, 60, 60, 59, 59, 59, 58, 58, 57, 57, 56, 56, 55, 55, 54, 54, 53, 52, 52, 51, 51, 50, 49, 49, 48, 47, 47, 46, 45, 44, 44, 43, 42, 42, 41, 40, 39, 39, 38, 37, 36, 35, 35, 34, 33, 32, 31, 31, 30, 29, 28, 28, 27, 26, 25, 24, 24, 23, 22, 21, 21, 20, 19, 19, 18, 17, 16, 16, 15, 14, 14, 13, 12, 12, 11, 11, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31
};

static inline u8 tsin(u8 x)
{
  return pgm_read_byte(&sin_tab[x]);
}

const u8 diagram_overlay[] PROGMEM =
{
	#include "wobble.h"
};

unsigned char counter=0;

static u8 table[64];
unsigned char cnt=0;
static GfxApiCompressedLayer Overlay;
unsigned char calc_wobble(unsigned char x,unsigned char y)
{
	
	if(y==0)
	{
		u8 shift=(tsin((x*3+counter)&0xff)+tsin((x*5+3*counter)&0xff))>>3;
		Overlay.SkipCounter+=shift;
	}
	if(y==63)
	{
		u8 shift=(tsin((x*3+counter)&0xff)+tsin((x*5+3*counter)&0xff))>>3;
		Overlay.SkipCounter-=shift;
	}
  GfxApiLayerGetNextByte(&Overlay);
	
  return Overlay.PixelValue;
	
}


void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}


void loop()
{
		for (;;)
	{
		Overlay.Bitpos=1;
		Overlay.BytePos=&diagram_overlay[0];
		Overlay.PixelValue=0;
		Overlay.SkipCounter=0;

		Display(0, 0, 0,0);
		counter+=3;
	}

  }
