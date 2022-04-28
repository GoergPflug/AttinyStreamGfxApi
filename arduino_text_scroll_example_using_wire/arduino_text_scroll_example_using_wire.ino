#include <Wire.h>

#define ENABLE_WIRE
#define WIRE_SCREEN_ADDRESS 0x3C


// To get it running change the #include "/dev/...." to the place where you stored the header Files!

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
#define SECOND_CONSOLE_LINE_LENGTH 17
#define SECOND_CONSOLE_LINE_ENABE_SCROLL
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define ENABLE_SECOND_CONSOLE

#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define SECOND_CONSOLE_LINE_START (0*8)

////////
#define ENABLE_FONT_BASIC
#define ENABLE_ARDUINO_SUPPORT
#include "/AttinyStreamGfxApi-main/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/AttinyStreamGfxApi-main/displayfunc.h"
/////////////////////////////////////////////////////////////////////

const  char s[] __attribute__((progmem)) ="                the quick brown fox jumps over the lazy dog                ";

void setup()
{
  streamgfx_init_arduino();
}

void loop()
{
  static unsigned char console[16*8];
  static unsigned char console2[SECOND_CONSOLE_LINE_LENGTH*8];
  memset(console2,ApiCharToFontIndex(' '),sizeof(console2));
  SystemServer_WriteToScreen("we come in peace",console,0,0);
  int scrollX=0;
  int scrollY=0;
  u16 zoom=256;

  int i;for(i=0x20;i<128;i++)console[i-0x20+16]=i-0x20;
  for(;;)
  {
    if(scrollX>(75-17)*8)
    {
      scrollX=0;
      zoom-=64;
      if(zoom<32)zoom=256;
    }
    else scrollX++;    
    int p=scrollX>>3;
    for(char i=0;i<17;i++)console2[i+17]=ApiCharToFontIndex(pgm_read_byte(&s[i+p]));
    Display(0, 0, 0,console,console2,zoom,zoom,(scrollX&7)*256,0);
  }
}
