// to get it running: change the #include "/dev/tiny_multi_os.h" and #include "/dev/displayfunc.h" to the place where you stored these files!
// change #define WIRE_SCREEN_ADDRESS if your Display uses a different I2C address
// this suffers a bit on the arduino, because there is no support for fixed point, so it seems it is actually running faster on the attiny in embedded c.


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
//#define ENABLE_CONSOLE
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384
//#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
unsigned char calc_plasma(unsigned char x,unsigned char y);
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK calc_plasma
#define ENABLE_LAYERS

#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
//#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
#define CONSOLE_SIZE_Y 4
#define CONSOLE_LINE_START (2*8)
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))

////////
#include "tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "displayfunc.h"
/////////////////////////////////////////////////////////////////////

const u8 star_overlay[] PROGMEM=
{
  #include "stern2.raw.h"
};

const unsigned char sin_tab[256]=
{
  32,32,33,34,35,35,36,37,38,39,39,40,41,42,42,43,44,44,45,46,47,47,48,49,49,50,51,51,52,52,53,54,54,55,55,56,56,57,57,58,58,59,59,59,60,60,60,61,61,61,62,62,62,62,63,63,63,63,63,63,63,63,63,63,64,63,63,63,63,63,63,63,63,63,63,62,62,62,62,61,61,61,60,60,60,59,59,59,58,58,57,57,56,56,55,55,54,54,53,52,52,51,51,50,49,49,48,47,47,46,45,44,44,43,42,42,41,40,39,39,38,37,36,35,35,34,33,32,31,31,30,29,28,28,27,26,25,24,24,23,22,21,21,20,19,19,18,17,16,16,15,14,14,13,12,12,11,11,10,9,9,8,8,7,7,6,6,5,5,4,4,4,3,3,3,2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,11,11,12,12,13,14,14,15,16,16,17,18,19,19,20,21,21,22,23,24,24,25,26,27,28,28,29,30,31
};
unsigned char counter=0;
unsigned char calc_plasma(unsigned char x,unsigned char y)
{
  static unsigned char xp1 = 0,xp2 = 0;
  static signed char  xadd1 = -5, xadd2 = 9;
  static unsigned char yp1 = 0,yp2 = 0;
  static signed char yadd1 = 3,  yadd2 = -8;

  static unsigned char xp1_1, xp2_1;
  static unsigned char yp1_1, yp2_1;
  
  if((x==0)&&(y==0))
  {
    xp1_1 = xp1, xp2_1 = xp2;
    yp1_1 = yp1, yp2_1 = yp2;
    xp1 += yadd2;
    xp2 -= yadd1;
    yp1 += xadd2;
    yp2 -= xadd1;

  }
  if(y==0)
  {
    yp1_1 += yadd1;
    yp2_1 += yadd2;
    xp1_1 = xp1, xp2_1 = xp2;
  }

return (((sin_tab[xp1_1 += xadd1] + sin_tab[xp2_1 += xadd2]
    + sin_tab[yp1_1] + sin_tab[yp2_1])))>>3;
}

void loop()
{
  static GfxApiCompressedLayer Overlay;
  for (;;)
  {

    Overlay.Bitpos = 1;
    Overlay.BytePos = &star_overlay[0];
    Overlay.SkipCounter = 0;
    Overlay.PixelValue = 0;

    s8 a=counter&0x7;
    a=a-4;
    a=abs(a);

    Display(&Overlay,a,0,0);
    counter++;
  }
}


void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}
