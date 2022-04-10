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
unsigned char render_lines(unsigned char x,unsigned char y);
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
#define PIXEL_CALLBACK render_lines
//#define ENABLE_LAYERS
//#define ENABLE_CONSOLE
#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)
////////
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
/////////////////////////////////////////////////////////////////////
// this is NOT how you should design a 3D-Engine, one should use matrix multiplications, but it demonstrates how to draw lines


#pragma GCC optimize ("Ofast")

static unsigned char linebuffer[64];

static const u8 bit_to_set[]={1,2,4,8,16,32,64,128}; // gcc builds stupid code for 1<<x
static unsigned char lines[4 * 12];
u8 linepos= 0;
static void store_line(unsigned char x1, unsigned char y1)
{
  lines[linepos] = x1;
  lines[linepos+1] = y1;
  linepos += 2;
}

static unsigned char cur_seg=0;

static void setpixel(u8 x,u8 y)
{
  unsigned char x_seg = x & 0xf8;
  if (x_seg != cur_seg)
  return;
  linebuffer[y] |= bit_to_set[x&7];
}

static unsigned char need_clear=1,clear_start=0,clear_end=0;
// Bresenham
static void line(u8 x0, u8 y0, u8 x1, u8 y1) {
  if((x0&0xf8)>cur_seg+7) return ; // links grösser als cur seg... raus...
  if((x1&0xf8)<cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
  // kann das segment nicht schneiden
  need_clear=1;
  
  s8 dx = abs((int)x1 - x0);
  s8 dy = abs((int)y1 - y0);
  s8 sy=-1;
  if(y0<y1)
  {
    sy=1;
    if(clear_start>y0)clear_start=y0;
    if(clear_end<y1)clear_end=y1;
  }
  else
  {
    if(clear_start>y1)clear_start=y1;
    if(clear_end<y0)clear_end=y0;
  }
  
  s8 err = ((dx > dy) ? dx : -dy) / 2, e2;

  for (;;) {
    setpixel(x0, y0);
    if ((x0 == x1) && (y0 == y1)) break;
    e2 = err;
    if (e2 > -dx) { err -= dy; x0++; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define NUM_OF_INDICES 12
#define SCREEN_CENTER_X  (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y  (SCREEN_HEIGHT / 2)
#define OBJ_SCALE 1500
#define CAMERA_DISTANCE 10

float objX = 0;
float objY = 0;
float objZ = 10;
float rotationX = 0;
float rotationY = 0;

 const float vertices[] = { -1, -1, -1,
  1, -1, -1,
  1, 1, -1,
  -1, 1, -1,
  -1, -1, 1,
  1, -1, 1
  , 1, 1, 1,
-1, 1, 1 };

const u8 indices[] = {  0*3, 1*3,
                1*3, 2*3, 
                2*3, 3*3, 
                3*3, 0,
                4*3, 5*3,
                5*3, 6*3, 
                6*3, 7*3,
                7*3, 4*3, 
                0*3, 4*3, 
                1*3, 5*3, 
                2*3, 6*3, 
                3*3, 7*3 };

static void rotate(float x, float y, float angle,float s,float c, float *r1, float *r2)
{
  *r1 = x * c - y * s;
  *r2 = y * c + x * s;
}
void Calc3D() {
  linepos = 0;
  float srx = sin(rotationX);
  float crx = cos(rotationX);

  float sry = sin(rotationY);
  float cry = cos(rotationY);
  
  for (int i = 0; i < sizeof(indices); i++)
  {
    u8 a = indices[i];
    float x1, y1, z1;
    x1 = vertices[a + 0];
    y1 = vertices[a + 1];
    z1 = vertices[a + 2];

    rotate(x1, z1, rotationY,sry,cry,&x1,&z1);
    rotate(y1, z1, rotationX,srx,crx,&y1,&z1);

    x1 = (float)(x1 * OBJ_SCALE);
    y1 = (float)(y1 * OBJ_SCALE);

    x1 += objX;
    y1 += objY;
    z1 += objZ;
    float inverse=1.0f/(z1 * CAMERA_DISTANCE);
    x1 *=inverse;
    y1 *=inverse;

    x1 += SCREEN_CENTER_X;
    y1 += SCREEN_CENTER_Y;

    store_line(x1, y1);// , x2, y2);
  }
  rotationX += 0.11;
  rotationY += 0.122;
}
#define swapu8(x,y) { u8 tmp=x;x=y;y=tmp; }

void reorder_lines()
{
  u8 i;
  for(i=0;i<linepos;i+=4)
  {
    if(lines[i]>lines[i+2])
    {
      swapu8(lines[i],lines[i+2]);
      swapu8(lines[i+1],lines[i+3]);
    }
  }
}

unsigned char render_lines(unsigned char x,unsigned char y)
{
  static unsigned char cur_bit=0;
  if(y==0)
  {
    cur_bit=bit_to_set[x&7];
    cur_seg=x&0xf8;
    if((x&7)==0)
    {  // clear linebuffer for 8x64 block and fill with lines
      if(need_clear)
      {
        need_clear=0;
        memset(linebuffer+clear_start, 0,clear_end-clear_start+1);
        clear_start=64,clear_end=0;
      }
      cur_seg=x&0xf8;
      for(u8 i=0;i<linepos;i+=4)
        line(lines[i], lines[i+1], lines[i+2], lines[i+3]);
    }
  }
  if(linebuffer[y]&cur_bit)return 63;
  return 0;
}






void setup() {
  // put your setup code here, to run once:
  streamgfx_init_arduino();
}

void loop() {
  // put your main code here, to run repeatedly:
  for (;;)
  {
    Calc3D();
    reorder_lines();
    Display(0, 0, 0,0);
  }

}
