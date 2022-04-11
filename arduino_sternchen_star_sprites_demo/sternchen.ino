// to get it running: change the #include "/dev/tiny_multi_os.h" and #include "/dev/displayfunc.h" to the place where you stored these files!
// change #define WIRE_SCREEN_ADDRESS if your Display uses a different I2C address
// this suffers a bit on the arduino, because there is no support for fixed point, so it seems it is actually running faster on the attiny in embedded c.


#include <Wire.h>


#define ENABLE_ARDUINO_SUPPORT
#define ENABLE_WIRE
#define WIRE_SCREEN_ADDRESS 0x3C

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

//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384
//#define ENABLE_USERFONT 1

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
#define ENABLE_SPRITES
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
//#define NR_LAYERS 1
#define NR_SPRITES 20
//#define SUBTRACT_LAYER0

#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (4*8)
////////
#define DISPLAYFUNC Display
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"


static const u8 sternchen[] PROGMEM=
{
0b00001000,
0b00011100, 
0b01111111,
0b00110110,
0b00100010,
};

int x[NR_SPRITES],y[NR_SPRITES];

static char console[16];

GfxApiSprite sprites[NR_SPRITES];

void setup() {
  streamgfx_init_arduino();
  SystemServer_WriteToScreen("Wandering Star",console,0,0);
  int i;
  for(i=0;i<NR_SPRITES;i++)
  {
    x[i]=rand()%120;
    y[i]=rand()%58;
    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  for (;;)
  {
    int i;
    for(i=0;i<NR_SPRITES;i++)
    {
      y[i]++;
      if(y[i]>58)y[i]=0,x[i]=rand()%120;
      sprites[i].zeros=y[i]+x[i]*64;
      sprites[i].readpos_bit=1;
      sprites[i].readpos_byte=&sternchen[0];
      sprites[i].sprite_height=sizeof(sternchen);
    }
    
    Display(0, 0, sprites,console);
    
  }

}
