#include <math.h>
#include <stdlib.h>

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
#define INTERLEAVE
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


static inline void _hline(u8 x0, u8 y0, u8 x1, u8 y1,u8 c, u8 *linebuffer) {
	y0-=96;
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
static const u8 hl[8] PROGMEM ={1,1+2,1+2+4,1+2+4+8,1+2+4+16,1+2+4+8+16+32,1+2+4+8+16+32+64,1+2+4+8+16+32+64+128}
;	
	u8 l=pgm_read_byte(&hl[x1-x0])<<x0;
	u8 mask=~l;
	c&=l;
	linebuffer[y0]=(linebuffer[y0]&mask)|c;
	//for(;x0!=x1+1;x0++)linebuffer[y0]^=1<<x0;
}

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
		_hline(minx,y, maxx, y,c,linebuffer);    // Draw line from min to max points found on the y
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
		_hline(minx,y, maxx, y,c,linebuffer);    // Draw line from min to max points found on the y
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
	s8 error_right[64];
	memset(error_right,0,sizeof(error_right));
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
	   }
#endif
#ifdef ENABLE_CONSOLE
		u8 charpos = x_pos_screen >> 3;
#endif
#ifdef ENABLE_SECOND_CONSOLE
#ifndef ENABLE_SECOND_CONSOLE_AFFINE
		u8 charpos_second_con = (second_console_x>>11);
		s16 second_console_y=scrollY;
#else
		s16 second_console_y=second_console_y0;
		s16 second_console_x_affine=second_console_x;
#endif
#endif
		u8 or_bit=1;
		s8 propagte_error=0;
		u8 block_8_px=0;
		u8 y_pos_screen;
		for (y_pos_screen = 0; y_pos_screen < 64; y_pos_screen++
#ifdef ENABLE_SECOND_CONSOLE
		,second_console_y+=zoomY
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
		,second_console_x_affine+=zoomX2
#endif
#endif
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
// vermutlich bug: (y_pos_screen>>8)
			if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen>>8)<SECOND_CONSOLE_LINE_END))
			{
				u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8)-SECOND_CONSOLE_LINE_START)>>3)]);
				const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x>>8) & 7)]);
				if(font_block8 & (1<<((second_console_y>>8)&7)))
					block_8_px|=or_bit;
			}
#else
			if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen>>8)<SECOND_CONSOLE_LINE_END))
			{
				u8 charpos_second_con = (second_console_x_affine>>11);
				u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8)-SECOND_CONSOLE_LINE_START)>>3)]);
				const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x_affine>>8) & 7)]);
				if(font_block8 & (1<<((second_console_y>>8)&7)))
				block_8_px|=or_bit;
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
				or_bit=1;
				#ifdef ENABLE_CONSOLE
				if((y_pos_screen>=CONSOLE_LINE_START)&&(y_pos_screen<CONSOLE_LINE_END))
				{
					u8 the_char = (screen[charpos +16* ((y_pos_screen-CONSOLE_LINE_START)>>3)]);
					block_8_px |= pgm_read_byte(&os_font[((int)the_char << 3) + (x_pos_screen & 7)]);
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
}
#undef INTERLEAVE
#ifdef DISABLE_HALFTONE
#undef DISABLE_HALFTONE
#endif
#undef STRINGNIZE
#undef STRINGNIZE_NX

#pragma GCC pop_options
