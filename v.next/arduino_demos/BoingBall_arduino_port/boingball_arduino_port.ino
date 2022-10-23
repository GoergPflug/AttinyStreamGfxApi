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
#define GFX_OPTIMIZE "Ofast"

static inline unsigned char calc_grid(unsigned char x,unsigned char y); 

//#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_grid
#define ENABLE_LAYERS
#define LAYERS_COLORKEY
//#define ENABLE_CONSOLE
#define NR_LAYERS 4
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)
////////
#include "tiny_multi_os.h"

 static inline unsigned char calc_grid(unsigned char x,unsigned char y)
{
	if(!(x&15))return 63;
	if(!(y&15))return 63;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////
const u8 diagram_overlay[] PROGMEM =
{
	#include "ball.h"
};

unsigned char counter=0;
u8 col=0;


void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}


void loop()
{
	static GfxApiCompressedLayer Overlay[NR_LAYERS];
	counter++;
	
	float a=0;
	for (;;)
	{
		int i;
		for(i=0;i<NR_LAYERS;i++)
		{
			int x=sin(a*(1-i/10.0))*50+50;
			int y=cos(a*(1-i/11.0))*16+16;
			
			Overlay[i].Bitpos = 1;
			Overlay[i].BytePos = &diagram_overlay[0];
			Overlay[i].SkipCounter = y+64*x;
			Overlay[i].PixelValue = 0;
		}
		Display(&Overlay[0], 0, 0,0);
		a+=0.05;
		//col++;
		col&=0xf;
	}
  
}
