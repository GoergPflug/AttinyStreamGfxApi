// to get it running change the #include paths form "/dev/" to where you store the library
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
#define ENABLE_LINEDRAWING
#define ENABLE_TRIANGLES   // line drawing must be enabled for triangles to work
#define NR_LINES 0
#define NR_TRIS (6*2)
//#define ENABLE_MULTITASK
#define OS_MAX_TASKS 2
#define OS_TASK_STACK 384

///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////

#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
//#define ENABLE_SPRITES
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
// this is NOT how you should design a 3D-Engine, but it demonstrates how to draw lines


#pragma GCC optimize ("Ofast")

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256
#define NUM_OF_INDICES 12
#define SCREEN_CENTER_X  (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y  (SCREEN_HEIGHT / 2)
#define OBJ_SCALE 2500
#define CAMERA_DISTANCE 10

accum objX = 0;
accum objY = 0;
accum objZ = 10;
accum rotationX = 0;
accum rotationY = 0;

__flash const accum vertices[] = {
	-1.0f,-1.0f,-1.0f, 
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 
	1.0f, 1.0f,-1.0f, 
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, 
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};

static void rotate(accum x, accum y, accum angle,accum s,accum c, accum *r1, accum *r2)
{
	*r1 = x * c - y * s;
	*r2 = y * c + x * s;
}
static __flash const u8 tri_color[12]={// 0,0,0,0,0,0,
										0x55,255,0x88,255,0x55,0x88,
										0x88,0x55,0x55,255,255,0x88,
									 };
typedef struct triangle
{
	u8 x1,y1,x2,y2,x3,y3,c,z;	
}triangle;

void gnomeSort(int n)
{
	triangle * arr=_gfx_points_of_lines;
	int index = 0;
	
	while (index < n) {
		if (index == 0)
		index++;
		if (arr[index].z <= arr[index - 1].z)
		index++;
		else {
			triangle tmp=arr[index];
			arr[index]=arr[index-1];
			arr[index-1]=tmp;
			index--;
		}
	}
	return;
}

void Calc3D() {
	GfxApiBeginTriangles();
	accum srx = sin(rotationX);
	accum crx = cos(rotationX);

	accum sry = sin(rotationY);
	accum cry = cos(rotationY);
	accum z_sort=0;
	u8 cnt=3,cnt2=0;
	for (int i = 0; i < 3*12; i++)
	{
		u8 a = i*3;
		accum x1, y1, z1;
		x1 = vertices[a + 0];
		y1 = vertices[a + 1];
		z1 = vertices[a + 2];

		rotate(x1, z1, rotationY,sry,cry,&x1,&z1);
		rotate(y1, z1, rotationX,srx,crx,&y1,&z1);

		x1 = (accum)(x1 * OBJ_SCALE);
		y1 = (accum)(y1 * OBJ_SCALE);

		x1 += objX;
		y1 += objY;
		z1 += objZ;
		accum inverse=1.0k/(z1 * CAMERA_DISTANCE);
		x1 *=inverse;
		y1 *=inverse;

		x1 += SCREEN_CENTER_X;
		y1 += SCREEN_CENTER_Y;
		z_sort+=z1*7;
		GfxApiStoreTrianglePoint(x1, y1);// , x2, y2);
		cnt--;
		if(!cnt)
		{
			cnt=3;
			
			//short _Fract z=z_sort;
			u8 t=z_sort;
			GfxApiStoreTrianglePoint(tri_color[cnt2],t);
			cnt2++;
			z_sort=0;
		}
	}
	rotationX += 0.11;
	rotationY += 0.122;
}
#define swapu8(x,y) { u8 tmp=x;x=y;y=tmp; }

extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()
{
	for (;;)
	{
		Calc3D();
		gnomeSort(12);
		Display(0, 0, 0,0);
	}
}
