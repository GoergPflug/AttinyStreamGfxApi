#include <math.h>
#include <stdlib.h>
//#define ENABLE_TRIANGLES
/* CPKI AttinyStreamGfxApi & TinyMultiOs, Version 0.8.2
see
https://www.youtube.com/watch?v=WNJQXsJqSbM
Copyright (c) 2002
Görg Pflug & CPKI Gmbh, www.cpki.de . All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the CPKI Gmbh, Görg Pflug and its contributors.”
Neither the name of the Cpki GmbH nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY Görg Pflug, CPKI Gmbh AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
Version:
0.8.1	faster
0.8		Filled Triangle drawing
0.5		faster, support functions, Consistent API, engine-interleaved I2C, propper line-drawing-api
		much better 3D-Performance
0.4		Prelimary Support for Arduino-Wire, performance worse than on attiny85
0.3		Compatible with C++, compiles in Microchip Studio and Arduino, most samples still untested
		added .INO example for Arduino
0.2		Multiple Consoles
0.2.1	ASCII Font Support
		define: ENABLE_FONT_BASIC
		"Low Quality" Halftoning, saves 64 bytes, faster, for some use cases "low quality" halftoning might even look better
		define: ENABLE_LOW_QUALITY_HALFTONE
		Halftoning gets disabled Automatically when not using layers or pixel callback, faster, saves 64 byte
		
*/
#ifndef ENABLE_WIRE
//#define INTERLEAVE
#endif
#pragma GCC push_options
#ifndef DISABLE_OFAST
#pragma GCC optimize ("Ofast")
#endif
#ifndef ENABLE_LAYERS
#ifndef PIXEL_CALLBACK
#define DISABLE_HALFTONE
#endif
#endif

#ifndef GfxApiLayerGetNextByteInlineHack
#define GfxApiLayerGetNextByteInlineHack

u8 select_dither=0;
const u8 _ordered_dither_matrix_simulated_annealing[] PROGMEM=
{
// this seems to be the best matrix for vector drawing
52,10,59,2,36,9,34,3,
20,38,23,49,19,62,22,48,
61,5,54,11,43,4,40,12,
25,42,17,46,27,57,18,45,
53,1,58,32,15,33,50,8,
21,31,39,7,63,0,30,37,
6,56,13,47,16,44,14,60,
41,24,35,29,55,26,51,28,



/*
  11,21,16,62,42,54,63,26,39,53,3,36,9,30,15,49,61,1,47,56,27,19,35,8,43,51,48,29,13,22,33,5,14,23,37,7,57,55,45,60,28,4,18,46,34,2,17,38,52,50,59,25,64,40,31,10,24,20,32,12,41,6,58,44,
	60,7,41,16,46,11,50,29,17,47,25,55,27,62,20,44,31,10,64,35,6,38,54,5,56,37,51,14,48,24,12,42,18,3,21,33,1,59,32,63,49,28,58,45,19,40,8,26,13,39,9,30,52,15,57,43,34,53,22,61,4,36,23,2,

	50,10,56,0,48,8,58,3,29,34,25,37,30,33,24,38,5,52,13,61,4,54,12,62,42,20,45,21,41,17,47,16,9,57,1,49,7,59,27,51,39,26,35,28,32,36,2,31,18,44,22,40,19,46,23,43,63,6,53,11,60,15,55,14,
	30,62,0,27,37,15,44,20,59,8,34,56,18,29,58,3,55,24,10,45,6,60,32,19,23,61,33,4,43,12,41,53,2,22,48,39,35,51,14,26,17,36,47,7,49,28,1,57,9,50,25,16,46,5,38,31,54,13,40,42,21,63,11,52,
	18,60,29,1,45,14,35,54,41,11,36,56,20,58,27,0,3,47,26,9,43,5,39,51,62,4,53,31,19,49,23,10,59,40,16,52,12,25,50,17,8,24,57,21,32,42,2,46,33,44,15,37,63,13,55,28,30,6,48,22,34,61,7,38,
*/
	/*g//bayer
			0,48,12,60, 3,51,15,63,
		   32,16,44,28,35,19,47,31,
			8,56, 4,52,11,59, 7,55,
		   40,24,36,20,43,27,39,23,
			2,50,14,62, 1,49,13,61,
		   34,18,46,30,33,17,45,29,
		   10,58, 6,54, 9,57, 5,53,
		   42,26,38,22,41,25,37,21 ,
/ */
//22,62,10,41,1,46,26,37,43,14,48,25,58,15,60,19,4,57,31,5,35,42,8,49,32,39,18,52,13,29,54,17,50,3,63,23,47,2,38,24,11,30,40,16,34,61,12,59,36,56,6,51,9,27,45,20,0,28,44,21,55,33,7,53,
//52,40,17,35,56,6,60,13,4,30,49,8,31,38,28,34,26,62,20,44,14,50,2,55,41,10,51,3,57,22,42,18,15,47,23,39,16,48,7,59,33,5,54,9,61,29,37,27,63,36,21,45,32,12,53,0,25,11,58,1,24,46,19,43,
//40,24,46,21,49,14,45,16,10,61,4,33,8,39,23,51,41,18,37,59,28,55,6,30,0,52,26,12,43,17,36,58,47,20,35,54,1,63,25,15,9,62,5,31,48,13,50,34,44,22,42,19,32,38,7,29,2,53,11,56,3,60,27,57,

//56,21,39,57,1,50,7,36,0,49,16,28,46,13,42,26,32,12,61,4,34,55,18,60,52,37,22,44,27,10,47,5,29,17,54,14,59,33,24,38,2,43,9,41,3,48,8,58,51,23,62,20,53,15,40,19,11,45,6,30,35,25,63,31,
//14,33,58,25,35,30,2,44,56,9,37,7,51,11,62,23,20,50,22,60,18,40,15,47,41,6,45,1,31,53,34,3,17,59,32,27,46,8,26,55,42,24,10,54,16,63,36,12,5,61,38,21,43,4,28,52,48,29,0,49,13,57,39,19,

//33,45,10,42,4,39,60,24,51,15,61,23,52,13,36,0,18,41,3,34,16,48,21,56,30,53,25,58,38,6,43,8,37,12,44,9,26,55,19,63,1,49,22,62,35,2,46,29,59,31,5,40,14,50,32,17,11,28,54,20,57,27,7,47,

// gut:
//12,39,4,43,10,23,56,36,60,24,54,19,63,32,7,30,2,42,8,35,14,46,29,47,51,20,59,28,52,3,58,15,9,38,5,41,17,48,22,45,62,25,53,21,61,13,34,16,0,44,11,37,1,31,55,33,50,18,57,26,49,40,6,27,
//0,33,11,54,2,58,12,38,24,56,37,19,42,16,32,62,43,8,29,63,7,51,28,5,15,50,35,14,44,20,36,55,47,1,27,53,3,59,9,31,30,57,34,18,41,25,45,17,10,40,6,61,13,52,4,60,49,21,48,26,39,22,46,23,
/*
52,10,59,2,36,9,34,3,
20,38,23,49,19,62,22,48,
61,5,54,11,43,4,40,12,
25,42,17,46,27,57,18,45,
53,1,58,32,15,33,50,8,
21,31,39,7,63,0,30,37,
6,56,13,47,16,44,14,60,
41,24,35,29,55,26,51,28,
*/
/*
62,35,2,42,4,46,34,3,
12,21,56,22,53,29,16,47,
51,31,43,11,36,7,58,23,
41,8,25,61,20,49,33,1,
19,55,38,0,45,9,27,60,
44,5,28,52,17,57,37,13,
18,59,32,10,40,30,6,50,
39,14,48,24,63,15,54,26,
/*
// anneal 2
3,42,7,46,34,5,62,35,
57,22,53,29,16,47,12,21,
43,11,36,1,58,23,51,31,
25,61,20,49,33,9,41,0,
38,8,45,6,27,60,19,55,
28,52,17,56,37,13,44,4,
32,10,40,30,2,50,18,59,
48,24,63,15,54,26,39,14,
*/
};  // ordered dither matrix generated by simulated annealing


// gcc is too stupid to inline this function..... because it is compiled on O3...
// so split.... but we want to define it only once, because the display function can potentially be included multiple times
// one should check this for the sprites too, the compiler pushed loads of variables.
static inline s8  __attribute__((always_inline)) GfxApiLayerGetNextByte (GfxApiCompressedLayer *g)
{
	if (g->SkipCounter)
	{
		g->SkipCounter--;
		return 0;
	} else return _GfxApiLayerGetNextByte(g);
}
#ifdef ENABLE_LINEDRAWING

void _setpixel(u8 x,u8 y,u8 *linebuffer)
{
	y-=96;
	if(y&(~63))return;//y-clipping

	unsigned char x_seg = x & 0xf8;
	if (x_seg != _cur_seg)
	return;
	//static u8 cnt;
	linebuffer[y] |=1<<(x&7);
//linebuffer[10]=cnt++;
}

static unsigned char _need_clear=1;//,_clear_start=0,_clear_end=0;
// Bresenham
static inline void _line(u8 x0, u8 y0, u8 x1, u8 y1,unsigned char *linebuffer) {

	if((x0&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
	if((x1&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
	// kann das segment nicht schneiden
	_need_clear=1;
	s8 dx =  abs(x1-x0);//, sx = x0<x1 ? 1 : -1;
	s8 dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = 0+dx+dy, e2; 
	
	for(;;){  
		_setpixel(x0,y0,linebuffer);
		if ((x0==x1) && (y0==y1)) return;
		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0++; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}	
}


static inline void _hline(u8 x0, u8 y0, u8 x1, u8 c, u8 *linebuffer) {
	y0-=96;
	if(x0>x1)swapu8(x0,x1);
	if(y0&(~63))return;//y-clipping
	if((x0&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
	if((x1&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
	// kann das segment nicht schneiden
	_need_clear=1;
// 	if(_clear_start>y0)_clear_start=y0;
// 	if(_clear_end<y0)_clear_end=y0;
	//_clear_start=0;
	//_clear_end=63;
	if(x0<_cur_seg)x0=_cur_seg;
	if(x1>_cur_seg+7)x1=_cur_seg+7;
	x0-=_cur_seg;
	x1-=_cur_seg;
	u8 a=1<<x0;
static const u8 hl[8] PROGMEM ={1,1+2,1+2+4,1+2+4+8,1+2+4+8+16,1+2+4+8+16+32,1+2+4+8+16+32+64,1+2+4+8+16+32+64+128}
;	
	u8 l=pgm_read_byte(&hl[x1-x0])<<x0;
	u8 mask=~l;
//	c&=l;
	linebuffer[y0]=(linebuffer[y0]&mask);//|c;
	u8 y_add=(y0&7)*8;
	u16 addr=(u16)&_ordered_dither_matrix_simulated_annealing[(x0&7)+y_add];
	u8 bit=1<<x0;
	for(;x0!=x1+1;x0++,addr++,bit<<=1)if(pgm_read_byte(addr)>c) linebuffer[y0]|=bit;
}

#ifdef ENABLE_CIRCLES
void drawCircle(int xc, int yc, int x, int y,u8 *linebuffer,u8 pat)
{
	
		_hline(xc-x,yc+y,xc+x,pat,linebuffer);
		_hline(xc-x,yc-y,xc+x,pat,linebuffer);
		_hline(xc+y,yc+x,xc-y,pat,linebuffer);
		_hline(xc+y,yc-x,xc-y,pat,linebuffer);
	
}
void fillCircle(u8 xc,u8 yc,u8 r,u8 pat,u8*linebuffer)
{
	if(((xc-r)&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
	if(((xc+r)&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
// kann das segment nicht schneiden
	_need_clear=1;
	int x = 0, y = r;
	int  d = 3 - 2 * r;
	drawCircle(xc, yc, x, y,linebuffer,pat);
	while (y >= x)
	{
		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		d = d + 4 * x + 6;
		drawCircle(xc, yc, x, y,linebuffer,pat);
	}
}
#endif

#ifdef ENABLE_TRIANGLES
// triangle code was once based on https://www.avrfreaks.net/sites/default/files/triangles.c , https://www.avrfreaks.net/forum/algorithm-draw-filled-triangle

void fillTriangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 x3,u8 y3, u8 c,u8*linebuffer) {
	u8 t1x,t2x,y,minx,maxx,t1xp,t2xp;
	u8 change=0;
	
	minx=x1;
	if(x2<minx)minx=x2;
	if(x3<minx)minx=x3;
	maxx=x1;
	if(x2>maxx)maxx=x2;
	if(x3>maxx)maxx=x3;
	if((minx&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
	if((maxx&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie


	s8 signx1,signx2,dx1,dy1,dx2,dy2;
	u8 e1,e2;
	// Sort vertices
	if (y1>y2) 
	{ 
		swapu8(y1,y2); 
		swapu8(x1,x2); 
	}
	if (y1>y3) 
	{ 
		swapu8(y1,y3); 
		swapu8(x1,x3); 
	}
	if (y2>y3) 
	{ 
		swapu8(y2,y3); 
		swapu8(x2,x3); 
	}

	t1x=x1;
	t2x=x1; 
	
	y=y1;   // Starting points

	dx1 = (s8)(x2 - x1); 
	if(dx1<0) 
	{ 
		dx1=-dx1; 
		signx1=-1; 
	} 
	else 
		signx1=1;
	dy1 = (s8)(y2 - y1);
	
	dx2 = (s8)(x3 - x1); 
	if(dx2<0) 
	{ 
		dx2=-dx2; 
		signx2=-1; 
	} 
	else signx2=1;
	
	dy2 = (s8)(y3 - y1);
	
	if (dy1 > dx1) 
	{   // swap values
		swapu8(dx1,dy1);
		change|=1;// = true;
	}
	if (dy2 > dx2) 
	{   // swap values
		swapu8(dy2,dx2);
		change|=2;// = true;
	}
	
	e2 = (u8)(dx2>>1);
	// Flat top, just process the second half
	if(y1==y2) goto next;
	
	e1 = (u8)(dx1>>1);
	
	for (u8 i = 0; i < dx1;) 
	{
		t1xp=0; t2xp=0;
		if(t1x<t2x) 
		{ 
			minx=t1x; 
			maxx=t2x; 
		}
		else		
		{ 
			minx=t2x; 
			maxx=t1x; 
		}
		// process first line until y value is about to change
		while(i<dx1) 
		{
			i++;
			e1 += dy1;
			while (e1 >= dx1) 
			{
				e1 -= dx1;
				if (change&1) 
					t1xp=signx1;
				else          
					goto next1;
			}
			if (change&1) break;
			
			t1x += signx1;
		}
		// Move line
		next1:
		// process second line until y value is about to change
		while (1) 
		{
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (change&2) 
					t2xp=signx2;
				else          
					goto next2;
			}
			if (change&2)
				break;
			t2x += signx2;
		}
		next2:
		if(minx>t1x) minx=t1x; 
		if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; 
		if(maxx<t2x) maxx=t2x;
		_hline(minx,y, maxx, c,linebuffer);    // Draw line from min to max points found on the y
		// Now increase y
		if(!(change&1)) t1x += signx1;
		t1x+=t1xp;
		if(!(change&2)) t2x += signx2;
		t2x+=t2xp;
		y += 1;
		if(y==y2) break;
		
	}
	next:
	// Second half
	dx1 = (s8)(x3 - x2); 
	if(dx1<0) 
	{ 
		dx1=-dx1; 
		signx1=-1; 
	} 
	else 
		signx1=1;
	dy1 = (s8)(y3 - y2);
	t1x=x2;
	
	if (dy1 > dx1) 
	{   // swap values
		swapu8(dy1,dx1);
		change|=1;// = true;
	} 
	else 
		change&=2;//false;
	
	e1 = (u8)(dx1>>1);
	
	for (u8 i = 0; i<=dx1; i++) 
	{
		t1xp=0; t2xp=0;
		if(t1x<t2x) 
		{ 
			minx=t1x; 
			maxx=t2x; 
		}
		else		
		{ 
			minx=t2x; 
			maxx=t1x; 
		}
		// process first line until y value is about to change
		while(i<dx1) {
			e1 += dy1;
			while (e1 >= dx1) 
			{
				e1 -= dx1;
				if (change&1) 
				{ 
					t1xp=signx1; 
					break; 
				}
				else          
					goto next3;
			}
			if (change&1) 
				break;
			
			t1x += signx1;
			if(i<dx1) i++;
		}
		next3:
		// process second line until y value is about to change
		while (t2x!=x3) 
		{
			e2 += dy2;
			while (e2 >= dx2) 
			{
				e2 -= dx2;
				if(change&2) 
					t2xp=signx2;
				else          
					goto next4;
			}
			if (change&2)     
				break;
			
			t2x += signx2;
		}
		next4:

		if(minx>t1x) minx=t1x; 
		if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; 
		if(maxx<t2x) maxx=t2x;
		_hline(minx,y, maxx, c,linebuffer);    // Draw line from min to max points found on the y
		// Now increase y
		if(!(change&1)) t1x += signx1;
		t1x+=t1xp;
		if(!(change&2)) t2x += signx2;
		t2x+=t2xp;
		y += 1;
		if(y>y3) return;
	}
}
#endif
#endif
#endif

static void DISPLAYFUNC (
	GfxApiCompressedLayer * layers, 
	u8 fade, 
	GfxApiSprite * Sprites, 
	u8 * screen
#ifdef ENABLE_CONSOLE0_FONT_SWITCHING
,long long font_adress_console_0
#endif
#ifdef ENABLE_SECOND_CONSOLE
	,u8 * screen2, 
	s16 zoomX,
	s16 zoomY,
	s16 scrollX,
	s16 scrollY
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
	,s16 zoomX2,
	s16 zoomY2
#endif
	)
{
#ifdef ENABLE_ATTINY_POWER_MANAGER
	if(low_power_screen_disable)
	{
		_manage_battery();
		return;
	}
#endif
	select_dither+=64;
	if (select_dither==64*4)select_dither=0;
#ifdef ENABLE_WIRE
	u8 wire_cnt=0;
#else
	u8 clock_i2c=0;    
#endif
#ifdef ENABLE_LINEDRAWING
	u8 linebuffer[64];
	memset(linebuffer,0,sizeof(linebuffer));
	reorder_lines();
	
#endif
#ifdef ENABLE_LAYERS
	u16 layers_constant_counter=0;
#endif
#ifdef ENABLE_SPRITES
	u16 sprites_constant_counter=0;
#endif
	u8 x_pos_screen;
	os_gfx_start_display_transfer();

#ifndef DISABLE_HALFTONE
#ifndef ENABLE_LOW_QUALITY_HALFTONE
#ifndef ENABLE_ORDERED_DITHERING
	s8 error_right[64];
	memset(error_right,0,sizeof(error_right));
#endif
#endif
	s8 layersum=0;
#endif

#ifdef ENABLE_SECOND_CONSOLE
	s16 second_console_x=scrollX;
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
	s16 second_console_y0=scrollY;
#endif

	for (x_pos_screen = 0; x_pos_screen < 128; x_pos_screen++
#ifdef ENABLE_SECOND_CONSOLE
	,second_console_x+=zoomX
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
	,second_console_y0+=zoomY2
#endif
	)
	{
#ifdef ENABLE_LINEDRAWING
	   u8 line_cur_bit = 1<<(x_pos_screen&7);
	   if((x_pos_screen&7)==0)
	   {  // clear linebuffer for 8x64 block and fill with lines
		   if(_need_clear)
		   {
			   _need_clear=0;
			   memset(linebuffer, 0,64);
			//   _clear_start=64,_clear_end=0;
		   }
		   _cur_seg=(x_pos_screen&0xf8)+64;
		   
 		   for(u8 i=0;i<_gfx_linepos;i+=4)
	  		   _line(_gfx_points_of_lines[i], _gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],linebuffer);
#ifdef ENABLE_TRIANGLES
		   for(u8 i=_gfx_linepos;i<_gfx_linepos+_gfx_tripos;i+=8)
	 		   fillTriangle(_gfx_points_of_lines[i], _gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],_gfx_points_of_lines[i+4], _gfx_points_of_lines[i+5],_gfx_points_of_lines[i+6],linebuffer);
#endif		 	   
#ifdef ENABLE_CIRCLES

#ifndef ENABLE_TRIANGLES
#define _gfx_tripos 0
#endif
		   for(u8 i=_gfx_linepos+_gfx_tripos;i<_gfx_linepos+_gfx_circlepos+_gfx_tripos;i+=4)
		   fillCircle(_gfx_points_of_lines[i], _gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],linebuffer);
#ifndef ENABLE_TRIANGLES
#undef _gfx_tripos
#endif
#endif
	   }
#endif
#ifdef ENABLE_CONSOLE
		u8 charpos = x_pos_screen >> 3;
#endif
#ifdef ENABLE_SECOND_CONSOLE
#ifndef ENABLE_SECOND_CONSOLE_AFFINE
		u8 charpos_second_con = (second_console_x>>11);
	//	if(charpos_second_con>SECOND_CONSOLE_LINE_LENGTH)charpos_second_con=SECOND_CONSOLE_LINE_LENGTH-1;
		s16 second_console_y=scrollY;
#else
		s16 second_console_y=second_console_y0;
		s16 second_console_x_affine=second_console_x;
#endif
#endif
		u8 or_bit=1;
#ifndef ENABLE_ORDERED_DITHERING		
		s8 propagte_error=0;
#endif
		u8 block_8_px=0;
		u8 y_pos_screen;
#ifdef ENABLE_ORDERED_DITHERING
		u8 ordered_dither_y=0;
		u8 ordered_dither_x=(x_pos_screen&7)<<3;
#endif
		for (y_pos_screen = 0; y_pos_screen < 64; y_pos_screen++
		)
		{
#ifdef INTERLEAVE			
			if(clock_i2c)one_i2c_bit_0();
#endif
#ifndef DISABLE_HALFTONE
#ifndef PIXEL_CALLBACK
			s8 background_pixel=0;
#else
			s8 background_pixel=PIXEL_CALLBACK (x_pos_screen,y_pos_screen);
#endif
			layersum=background_pixel;
#endif
#ifdef ENABLE_LAYERS
			if(!layers_constant_counter)
			{
				u8 i;
				for(i=0;i<NR_LAYERS;i++)
				layers[i].PixelValue += GfxApiLayerGetNextByte(&layers[i]);
			}
			else layers_constant_counter--;
			// change to implement different combination logic for the layers
			
#ifndef LAYERS_COLORKEY
#ifndef SUBTRACT_LAYER0
			layersum+=layers[0].PixelValue>>fade;
#else
			layersum-=layers[0].PixelValue>>fade;
#endif
#endif
			
#ifndef LAYERS_COLORKEY
			for(u8 i=1;i<NR_LAYERS;i++)
				layersum+=layers[i].PixelValue;
#else
			for(u8 i=0;i<NR_LAYERS;i++)if(layers[i].PixelValue)
				layersum=layers[i].PixelValue;
#endif
#endif
			
#ifndef DISABLE_HALFTONE			
			if(layersum<0)layersum=0;
			if(layersum>63)layersum=63;
#ifndef ENABLE_ORDERED_DITHERING
#ifndef ENABLE_LOW_QUALITY_HALFTONE			
			propagte_error+=layersum+error_right[y_pos_screen];
#else
			propagte_error+=layersum;
#endif
			if (propagte_error > 31)block_8_px |= or_bit, propagte_error-=63;
#ifndef ENABLE_LOW_QUALITY_HALFTONE
			propagte_error/=2;
			error_right[y_pos_screen]=propagte_error;
#endif
#endif			
#endif

#ifdef ENABLE_ORDERED_DITHERING
			if (pgm_read_byte(&_ordered_dither_matrix_simulated_annealing[ordered_dither_y+ordered_dither_x])>layersum)block_8_px |= or_bit;
			ordered_dither_y++;
#endif
			
#ifdef ENABLE_SPRITES
			if(!sprites_constant_counter)
			{
				u8 i;
				for(i=0;i<NR_SPRITES;i++)
				if(GfxApiReadSprite(&Sprites[i]))block_8_px|=or_bit;
			}else sprites_constant_counter--;
#endif
			
#ifdef ENABLE_SECOND_CONSOLE
#ifndef ENABLE_SECOND_CONSOLE_AFFINE
	if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen)<SECOND_CONSOLE_LINE_END))
			{
				u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8))>>3)]);


//				u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8)-SECOND_CONSOLE_LINE_START)>>3)]);
				const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x>>8) & 7)]);
				if(font_block8 & (1<<((second_console_y>>8)&7)))
					block_8_px|=or_bit;
//				if(y_pos_screen&1)	
	//				block_8_px|=or_bit;
					
				second_console_y+=zoomY;
			}
#else
			if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen)<SECOND_CONSOLE_LINE_END))
			{
				u8 charpos_second_con = (second_console_x_affine>>11);
				if(charpos_second_con>SECOND_CONSOLE_LINE_LENGTH)charpos_second_con=SECOND_CONSOLE_LINE_LENGTH-1;
				u8 the_char = (screen2[charpos_second_con +(SECOND_CONSOLE_LINE_LENGTH)* (((second_console_y>>8))>>3)]);
				const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x_affine>>8) & 7)]);
				if(font_block8 & (1<<((second_console_y>>8)&7)))
					block_8_px|=or_bit;
				second_console_y+=zoomY;
				second_console_x_affine+=zoomX2;
			}
#endif
#endif
#ifdef ENABLE_LINEDRAWING
			if(linebuffer[y_pos_screen]&line_cur_bit)block_8_px|=or_bit;;
#endif
			or_bit<<=1;
#ifdef INTERLEAVE			
			if(clock_i2c)   // we use the render engine as "delay loop" for the i2c...
			{
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
				asm("cbi 0x18," STRINGIZE(PIN_SCL));
				asm("sbi 0x0d,1");
// 				PORTB &= ~_BV(PIN_SCL);
// 				USICR |= _BV(USICLK);
 			}
#endif			 
			if(!or_bit)
			{
#ifdef ENABLE_ORDERED_DITHERING
				ordered_dither_y=0;
#endif
				or_bit=1;
#ifdef ENABLE_CONSOLE
				if((y_pos_screen>=CONSOLE_LINE_START)&&(y_pos_screen<CONSOLE_LINE_END))
				{
					u8 the_char = (screen[charpos +16* ((y_pos_screen-CONSOLE_LINE_START)>>3)]);
#ifdef ENABLE_CONSOLE0_FONT_SWITCHING
				long long addr=pgm_get_far_address(os_font);
				addr+=((long long)the_char << 3) + (x_pos_screen & 7);
				addr+=font_adress_console_0;
#endif

#ifdef ENABLE_CONSOLE_BIT7_INVERT
					u8 xor_mask=(the_char&0x80)?255:0;
					the_char&=0x7f;
#endif								
					block_8_px |=
#ifndef ENABLE_CONSOLE0_FONT_SWITCHING				
					pgm_read_byte(&os_font[((int)the_char << 3) + (x_pos_screen & 7)])
#else
					pgm_read_byte_far(addr)
#endif
#ifdef ENABLE_CONSOLE_BIT7_INVERT
					^xor_mask
#endif								
					;
				}
				
#endif
//				PORTB=block_8_px;
#ifdef INTERLEAVE
				one_i2c_bit_byte(block_8_px);
				one_i2c_bit_0();
				clock_i2c=1;
#else
				os_i2c_write_byte(block_8_px);
#endif
				block_8_px = 0;
#ifdef INTERLEAVE
				one_i2c_bit_1();
#endif				
#ifdef ENABLE_LAYERS
				if(!layers_constant_counter)
				{
					u16 min=layers[0].SkipCounter;
					u8 i;
					for(i=1;i<NR_LAYERS;i++)
					{
						if(layers[i].SkipCounter<min)min=layers[i].SkipCounter;
					}
					if(min)
					{
						layers_constant_counter=min;
						for(i=0;i<NR_LAYERS;i++)
							layers[i].SkipCounter-=layers_constant_counter;
					}
				}
#endif
#ifdef ENABLE_SPRITES
				if(!sprites_constant_counter)
				{
					u16 min=Sprites[0].SkipCounter;
					u8 i;
					for(i=1;i<NR_SPRITES;i++)
					{
						if(Sprites[i].SkipCounter<min)min=Sprites[i].SkipCounter;
					}
					if(min)
					{
						sprites_constant_counter=min;
						for(i=0;i<NR_SPRITES;i++)
						Sprites[i].SkipCounter-=sprites_constant_counter;
					}
				}
#endif
			}  // ende 8 pixel block
		}   // ende y schleife
#ifdef ENABLE_WIRE
		wire_cnt++;
		if (wire_cnt==3)
		{
			wire_cnt=0;	
			Wire.endTransmission();
			Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
			Wire.write(0x40);
		}
#endif
	}   // ende x schleife
#ifdef INTERLEAVE
	for (u8 i=0;i<8;i++)
	{
				one_i2c_bit_0();
				one_i2c_bit_1();
	}
#endif	
	os_i2c_stop();
#ifdef ENABLE_ATTINY_POWER_MANAGER
	_manage_battery();		
#endif

}
#undef INTERLEAVE
#ifdef DISABLE_HALFTONE
#undef DISABLE_HALFTONE
#endif
#undef STRINGNIZE
#undef STRINGNIZE_NX

#pragma GCC pop_options
