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
unsigned char calc_alien_bg(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_alien_bg
#define ENABLE_LAYERS
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

const u8 PROGMEM diagram_overlay[] =
{
#include "spaceship.h"
};

unsigned char rnd()
{
  static char tab[16] = { 0,1,2,3 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  static char a,b,c;
  a++;
  a &= 0x0f;
  c = tab[b];
  int tmp = tab[a];
  tab[a] = tab[b];
  tab[b] = tmp;
  b = c;
  return b;
}


unsigned char counter=0;

s8 table[64];
unsigned char cnt=0;
unsigned char calc_alien_bg(unsigned char x,unsigned char y)
{
  return rnd()<<2;
}

void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}


void loop()
{
  static GfxApiCompressedLayer Overlay;
  os_gfx_start_display_transfer();
  unsigned char flip=0;
  for (;;)
  {
    
    Overlay.Bitpos = 1;
    Overlay.BytePos = &diagram_overlay[0];
    Overlay.SkipCounter = 0;
    Overlay.PixelValue = 0;
    Display(&Overlay, (cnt>>2)&7, 0,0);
    cnt++;
  }
}
