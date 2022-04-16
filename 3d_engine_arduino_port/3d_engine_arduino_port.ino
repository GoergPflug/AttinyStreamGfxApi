#include <Wire.h>


#define ENABLE_ARDUINO_SUPPORT
#define ENABLE_WIRE
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
//#define PIXEL_CALLBACK render_lines
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

#define NUM_OF_INDICES 12
#define OBJ_SCALE 1500
#define CAMERA_DISTANCE 10

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
  GfxApiBeginLines();
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

    GfxApiStoreLinePoint(x1, y1);// , x2, y2);
  }
  rotationX += 0.11;
  rotationY += 0.122;
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
    Display(0, 0, 0,0);
  }
}
