/* CPKI AttinyGfxApi & TinyMultiOs, Preview Version 0.9
see
https://www.youtube.com/watch?v=WNJQXsJqSbM
Copyright (c) 2002
Görg Pflug & CPKI Gmbh, www.cpki.de . All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the CPKI Gmbh, Görg Pflug and its contributors.”
Neither the name of the Cpki GmbH nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE Görg Pflug, CPKI Gmbh AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

		
*/
#define DO_NOP asm volatile("nop");asm volatile("nop");asm volatile("nop");
#ifndef F_CPU 
#define F_CPU 16000000
#warning "CPU Clock: assuming 16mhz"
#endif
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/power.h>
#include <util/delay.h>

#define s8 signed char
#define u8 unsigned char
#define s16 signed int
#define u16 unsigned int



#define SSD1327_BLACK 0x0
#define SSD1327_WHITE 0xF

#define SSD1327_I2C_ADDRESS 0x3D

#define SSD1305_SETBRIGHTNESS 0x82

#define SSD1327_SETCOLUMN 0x15

#define SSD1327_SETROW 0x75

#define SSD1327_SETCONTRAST 0x81

#define SSD1305_SETLUT 0x91

#define SSD1327_SEGREMAP 0xA0
#define SSD1327_SETSTARTLINE 0xA1
#define SSD1327_SETDISPLAYOFFSET 0xA2
#define SSD1327_NORMALDISPLAY 0xA4
#define SSD1327_DISPLAYALLON 0xA5
#define SSD1327_DISPLAYALLOFF 0xA6
#define SSD1327_INVERTDISPLAY 0xA7
#define SSD1327_SETMULTIPLEX 0xA8
#define SSD1327_REGULATOR 0xAB
#define SSD1327_DISPLAYOFF 0xAE
#define SSD1327_DISPLAYON 0xAF

#define SSD1327_PHASELEN 0xB1
#define SSD1327_DCLK 0xB3
#define SSD1327_PRECHARGE2 0xB6
#define SSD1327_GRAYTABLE 0xB8
#define SSD1327_PRECHARGE 0xBC
#define SSD1327_SETVCOM 0xBE

#define SSD1327_FUNCSELB 0xD5

#define SSD1327_CMDLOCK 0xFD
unsigned char init1327_2[]
{
	0,

0xae,//--turn off oled panel

0x15,  //set column addresses
0x00,  //start column  0
0x7f,  //end column  127

0x75,  //set row addresses
0x00,  //start row  0
0x7f,  //end row  127

0x81,  //set contrast control
0x80,  //50% (128/255)

0xa0,   //gment remap
0x51,  //51 (To my understanding, this is orientation

0xa1,  //start line
0x00,

0xa2,  //display offset
0x00,

0xa4,  //rmal display
0xa8,  //set multiplex ratio
0x7f,

0xb1,  //set phase leghth
0xf1,

0xb3,  //set dclk
0x00,  //80Hz:0xc1 90Hz:0xe1  100Hz:0x00  110Hz:0x30 120Hz:0x50  130Hz:0x70   01

0xab,  //Enable vReg
0x01,

0xb6,  //set phase leghth
0x0f,

0xbe, //Set vcomh voltage
0x0f,

0xbc, //set pre-charge voltage
0x08,

0xd5, //second precharge period
0x62,

0xfd, //Unlock commands
0x12,

0xAF,
};

static const uint8_t init_128x128[] = {
	0,
      // Init sequence for 128x32 OLED module
      SSD1327_DISPLAYOFF, // 0xAE
      SSD1327_SETCONTRAST,
      0x80,             // 0x81, 0x80
      SSD1327_SEGREMAP, // 0xA0 0x53
      0x51, // remap memory, odd even columns, com flip and column swap
      SSD1327_SETSTARTLINE,
      0x00, // 0xA1, 0x00
      SSD1327_SETDISPLAYOFFSET,
      0x00, // 0xA2, 0x00
      SSD1327_DISPLAYALLOFF, SSD1327_SETMULTIPLEX,
      0x7F, // 0xA8, 0x7F (1/64)
      SSD1327_PHASELEN,
      0x11, // 0xB1, 0x11
      /*
      SSD1327_GRAYTABLE,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
      0x07, 0x08, 0x10, 0x18, 0x20, 0x2f, 0x38, 0x3f,
      */
      SSD1327_DCLK,
      0x00, // 0xb3, 0x00 (100hz)
      SSD1327_REGULATOR,
      0x01, // 0xAB, 0x01
      SSD1327_PRECHARGE2,
      0x04, // 0xB6, 0x04
      SSD1327_SETVCOM,
      0x0F, // 0xBE, 0x0F
      SSD1327_PRECHARGE,
      0x08, // 0xBC, 0x08
      SSD1327_FUNCSELB,
      0x62, // 0xD5, 0x62
      SSD1327_CMDLOCK,
      0x12, // 0xFD, 0x12
      SSD1327_NORMALDISPLAY, SSD1327_DISPLAYON};





#define SSD1306_ADDRESS 0x78
#define I2C_PORT PORTD
#define I2C_DDR DDRD
#define PIN_SDA  1
#define PIN_SCL 0
// helper function, Calculate Skip Counter from x,y coord
static inline int GfxApiPosition(unsigned char x, unsigned char y)
{
	return x*64+y;
}

#ifdef ENABLE_LINEDRAWING
u8 _gfx_linepos= 0;
#ifdef ENABLE_TRIANGLES
u8 _gfx_tripos=0;
#endif
#ifdef ENABLE_CIRCLES
u8 _gfx_circlepos=0;
#endif


#ifndef NR_TRIS
#define NR_TRIS 0
#endif
#ifndef NR_CIRCLES
#define NR_CIRCLES 0
#endif
static unsigned char _gfx_points_of_lines[4 * NR_LINES+ 8 * NR_TRIS+4*NR_CIRCLES];

// Start storage of Line Points
static void GfxApiBeginLines()
{
	_gfx_linepos=0;
}
#ifdef ENABLE_TRIANGLES
static void GfxApiBeginTriangles()
{
	_gfx_tripos=0;
}
#endif
#ifdef ENABLE_CIRCLES
static void GfxApiBeginCircles()
{
	_gfx_circlepos=0;
}
#endif

#define swapu8(x,y) { u8 tmp=x;x=y;y=tmp; }
void reorder_lines()
{
	u8 i;
	for(i=0;i<_gfx_linepos;i+=4)
	{
		if(_gfx_points_of_lines[i]>_gfx_points_of_lines[i+2])
		{
			swapu8(_gfx_points_of_lines[i],_gfx_points_of_lines[i+2]);
			swapu8(_gfx_points_of_lines[i+1],_gfx_points_of_lines[i+3]);
		}
	}
}

static void GfxApiStoreLinePoint(unsigned char x1, unsigned char y1)
{
	_gfx_points_of_lines[_gfx_linepos] = x1;
	_gfx_points_of_lines[_gfx_linepos+1] = y1;
	_gfx_linepos += 2;
}

#ifdef ENABLE_CIRCLES
static void GfxApiStoreCircle(unsigned char x1, unsigned char y1,unsigned char radius,unsigned char pattern)
{
#ifndef ENABLE_TRIANGLES
#define _gfx_tripos 0
#endif
	_gfx_points_of_lines[_gfx_circlepos+_gfx_linepos+_gfx_tripos] = x1;
	_gfx_points_of_lines[_gfx_circlepos+1+_gfx_linepos+_gfx_tripos] = y1;
	_gfx_points_of_lines[_gfx_circlepos+2+_gfx_linepos+_gfx_tripos] = radius;
	_gfx_points_of_lines[_gfx_circlepos+3+_gfx_linepos+_gfx_tripos] = pattern;
	_gfx_circlepos += 4;

#ifndef ENABLE_TRIANGLES
#undef _gfx_tripos
#endif
}
#endif

#ifdef ENABLE_TRIANGLES
static void GfxApiStoreTrianglePoint(unsigned char x1, unsigned char y1)
{
	_gfx_points_of_lines[_gfx_linepos+_gfx_tripos] = x1;
	_gfx_points_of_lines[_gfx_linepos+1+_gfx_tripos] = y1;
	_gfx_tripos += 2;
}
#endif
#endif
#ifdef ENABLE_LINEDRAWING
static unsigned char _cur_seg=0;
#endif

void ApiIntToHex(u16 in, u8 *out);
u8 ApiCharToFontIndex(u8 c);

typedef struct GfxApiSprite
{
	u16 SkipCounter;
	u8 readpos_bit;
	u16 readpos_byte;
	u8 sprite_height;
}GfxApiSprite;

static u8 GfxApiReadSprite(GfxApiSprite *s);

typedef struct GfxApiCompressedLayer
{
	u16 SkipCounter;
	s8 PixelValue;
	u8 Bitpos;
	u16 BytePos;
} GfxApiCompressedLayer;

static s8 os_gfx_layer_read_vlc(GfxApiCompressedLayer *g);
static void os_gfx_start_display_transfer();
static void os_i2c_start (void);



#define os_i2c_scl_high()	do{ I2C_DDR |= (1<<PIN_SCL);} while (0)
#define os_i2c_sda_high()	do{	I2C_DDR |= (1<<PIN_SDA);} while(0)
#define os_i2c_sda_low()	do {I2C_DDR &= ~(1<<PIN_SDA);} while(0)
#define os_i2c_scl_low() 	do{ I2C_DDR &= ~(1<<PIN_SCL); } while(0)



#define os_i2c_scl_high()	do{ I2C_PORT |= (1<<PIN_SCL);} while (0)
#define os_i2c_sda_high()	do{	I2C_PORT |= (1<<PIN_SDA);} while(0)
#define os_i2c_sda_low()	do {I2C_PORT &= ~(1<<PIN_SDA);} while(0)
#define os_i2c_scl_low() 	do{ I2C_PORT &= ~(1<<PIN_SCL); } while(0)



#define os_i2c_dbg_high() 	do{ I2C_DDR &= ~(1<<5); } while(0)
#define os_i2c_dbg_low()    do{I2C_DDR |= (1<<5);} while (0)

void os_i2c_init (void);
static void os_i2c_write (const u8 *buf, u8 len);

#ifdef ENABLE_CONSOLE
#define FONT
#endif
#ifdef ENABLE_SECOND_CONSOLE
#define FONT
#endif
#ifdef FONT
const u8 os_font[] __attribute__((progmem)) = {
#ifdef REPLACE_FONT
#include REPLACE_FONT
#else
#ifdef ENABLE_FONT_BASIC
// source https://raw.githubusercontent.com/dhepper/font8x8/master/font8x8_basic.h , public domain
0,0,0,0,0,0,0,0,0,0,6,95,95,6,0,0,0,3,3,0,3,3,0,0,20,127,127,20,127,127,20,0,36,46,107,107,58,18,0,0,70,102,48,24,12,102,98,0,48,122,79,93,55,122,72,0,4,7,3,0,0,0,0,0,0,28,62,99,65,0,0,0,0,65,99,62,28,0,0,0,8,42,62,28,28,62,42,8,8,8,62,62,8,8,0,0,0,128,224,96,0,0,0,0,8,8,8,8,8,8,0,0,0,0,96,96,0,0,0,0,96,48,24,12,6,3,1,0,62,127,113,89,77,127,62,0,64,66,127,127,64,64,0,0,98,115,89,73,111,102,0,0,34,99,73,73,127,54,0,0,24,28,22,83,127,127,80,0,39,103,69,69,125,57,0,0,60,126,75,73,121,48,0,0,3,3,113,121,15,7,0,0,54,127,73,73,127,54,0,0,6,79,73,105,63,30,0,0,0,0,102,102,0,0,0,0,0,128,230,102,0,0,0,0,8,28,54,99,65,0,0,0,36,36,36,36,36,36,0,0,0,65,99,54,28,8,0,0,2,3,81,89,15,6,0,0,62,127,65,93,93,31,30,0,124,126,19,19,126,124,0,0,65,127,127,73,73,127,54,0,28,62,99,65,65,99,34,0,65,127,127,65,99,62,28,0,65,127,127,73,93,65,99,0,65,127,127,73,29,1,3,0,28,62,99,65,81,115,114,0,127,127,8,8,127,127,0,0,0,65,127,127,65,0,0,0,48,112,64,65,127,63,1,0,65,127,127,8,28,119,99,0,65,127,127,65,64,96,112,0,127,127,14,28,14,127,127,0,127,127,6,12,24,127,127,0,28,62,99,65,99,62,28,0,65,127,127,73,9,15,6,0,30,63,33,113,127,94,0,0,65,127,127,9,25,127,102,0,38,111,77,89,115,50,0,0,3,65,127,127,65,3,0,0,127,127,64,64,127,127,0,0,31,63,96,96,63,31,0,0,127,127,48,24,48,127,127,0,67,103,60,24,60,103,67,0,7,79,120,120,79,7,0,0,71,99,113,89,77,103,115,0,0,127,127,65,65,0,0,0,1,3,6,12,24,48,96,0,0,65,65,127,127,0,0,0,8,12,6,3,6,12,8,0,128,128,128,128,128,128,128,128,0,0,3,7,4,0,0,0,32,116,84,84,60,120,64,0,65,127,63,72,72,120,48,0,56,124,68,68,108,40,0,0,48,120,72,73,63,127,64,0,56,124,84,84,92,24,0,0,72,126,127,73,3,2,0,0,152,188,164,164,248,124,4,0,65,127,127,8,4,124,120,0,0,68,125,125,64,0,0,0,96,224,128,128,253,125,0,0,65,127,127,16,56,108,68,0,0,65,127,127,64,0,0,0,124,124,24,56,28,124,120,0,124,124,4,4,124,120,0,0,56,124,68,68,124,56,0,0,132,252,248,164,36,60,24,0,24,60,36,164,248,252,132,0,68,124,120,76,4,28,24,0,72,92,84,84,116,36,0,0,0,4,62,127,68,36,0,0,60,124,64,64,60,124,64,0,28,60,96,96,60,28,0,0,60,124,112,56,112,124,60,0,68,108,56,16,56,108,68,0,156,188,160,160,252,124,0,0,76,100,116,92,76,100,0,0,8,8,62,119,65,65,0,0,0,0,0,119,119,0,0,0,65,65,119,62,8,8,0,0,2,3,1,3,2,3,1,0,0,0,0,0,0,0,0,0,
#define FONT_ASCII
#define HAVE_FONT
#endif
#ifndef HAVE_FONT  // minimal font 0-9,A-Z
	0,0,0,0,0,0,0,0,0,60,98,82,74,70,60,0,0,0,68,66,126,64,64,
	0,0,100,82,82,82,82,76,0,0,36,66,66,74,74,52,0,0,48,40,36,
	126,32,32,0,0,46,74,74,74,74,50,0,0,60,74,74,74,74,48,0,0,
	2,2,98,18,10,6,0,0,52,74,74,74,74,52,0,0,12,82,82,82,82,60,0,0,
	124,18,18,18,18,124,0,0,126,74,74,74,74,52,0,0,60,66,66,66,66,
	36,0,0,126,66,66,66,36,24,0,0,126,74,74,74,74,66,0,0,126,10,
	10,10,10,2,0,0,60,66,66,82,82,52,0,0,126,8,8,8,8,126,0,0,0,
	66,66,126,66,66,0,0,48,64,64,64,64,62,0,0,126,8,8,20,34,
	64,0,0,126,64,64,64,64,64,0,0,126,4,8,8,4,126,0,0,126,4,8,16,
	32,126,0,0,60,66,66,66,66,60,0,0,126,18,18,18,18,12,0,0,60,66,82,98,
	66,60,0,0,126,18,18,18,50,76,0,0,36,74,74,74,74,48,0,2,2,2,126,2,2,
	2,0,0,62,64,64,64,64,62,0,0,30,32,64,64,32,30,0,0,62,64,32,32,64,
	62,0,0,66,36,24,24,36,66,0,2,4,8,112,8,4,2,0,0,66,98,82,74,70,66,0,
#undef HAVE_FONT
#undef FONT 
#endif	

#ifdef ENABLE_USERFONT
#include "userfont.h"
#endif
#endif
};
#endif

#ifndef ENABLE_WIRE


/*
static inline void os_i2c_scl_high (void)
{
	 I2C_DDR &= ~(1<<PIN_SCL);
	//I2C_PORT |= _BV(PIN_SCL);
}
*/

#endif
static void os_i2c_start (void)
{
#ifndef ENABLE_WIRE
	os_i2c_scl_high();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");					
	os_i2c_sda_low();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");						
	os_i2c_scl_low();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	os_i2c_sda_high();
#else

#endif
}

static void os_i2c_stop (void)
{
#ifndef ENABLE_WIRE
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	os_i2c_sda_low();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	os_i2c_scl_high();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	os_i2c_sda_high();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
#else
	Wire.endTransmission();
#endif
}


static void os_i2c_write_byte (u8 byte)
{
#ifndef ENABLE_WIRE
	//USIDR = byte;
	//USISR = 7;
	u8 bitpos=0x80;
	
	do {
		if(byte&bitpos)
			os_i2c_sda_high();
		else
		{
			os_i2c_sda_low();
			asm volatile("nop");
			asm volatile("nop");
			asm volatile("nop");
		}
		os_i2c_scl_high();
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		os_i2c_scl_low();
		bitpos>>=1;
//		USICR |= _BV(USICLK);
	//	bitpos>>=1;
	} while (bitpos);
	//while (USISR & 0x0F);
	
	os_i2c_sda_high();						 
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	os_i2c_scl_high();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

	os_i2c_scl_low();
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");

#else
	Wire.write(byte);
#endif
}

void os_i2c_init (void)
{
#ifndef ENABLE_WIRE
	I2C_PORT = 0xff;//_BV(PIN_SDA) | _BV(PIN_SCL);
	I2C_DDR  = _BV(PIN_SDA) | _BV(PIN_SCL);
	os_i2c_scl_high();
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");

	os_i2c_sda_high();
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");
							 asm volatile("nop");

#else
	Wire.begin();
	Wire.setClock(800000);
#endif
}

static void os_i2c_write (const u8 *buf, u8 len)
{
#ifndef ENABLE_WIRE
	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	while (len--)
		os_i2c_write_byte(*buf++);
	os_i2c_stop();
#else
	Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
	while (len--)
		Wire.write(*buf++);
	Wire.endTransmission();
#endif
}
/*
FUSES =
{
	.low = LFUSE_DEFAULT,
	.high = HFUSE_DEFAULT,//(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE & FUSE_SPIEN ),
	.extended = FUSE_SELFPRGEN,
};
*/
const u8 os_conv_hex[] __attribute__((progmem)) = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void ApiIntToHex(u16 in, u8 *out)
{
	out[0] =pgm_read_byte(&os_conv_hex[(in >> 12) & 0xf]);
	out[1] =pgm_read_byte(&os_conv_hex[(in >> 8) & 0xf]);
	out[2] =pgm_read_byte(&os_conv_hex[(in >> 4) & 0xf]);
	out[3] =pgm_read_byte(&os_conv_hex[in & 0xf]);
}

u8 ApiCharToFontIndex(u8 c)
{
	#ifdef FONT_ASCII
		return c-0x20;
	#else
	if (c >= 'a' && c <= 'z')return 11 + c - 'a';
	if (c >= 'A' && c <= 'Z')return 11 + c - 'A';
	if (c >= '0' && c <= '9')return 1 + c - '0';
	return 0;
	#endif
}


// convert a C-String to Font Indices, can be used to use sprintf directly in the console buffer
// example:
// sprintf(myconsole,"hello world");
// GfxApiConvertStringToFontIdx(myconsole);
void GfxApiConvertStringToFontIdx(u8 *s)
{
	while(*s)
	{
		*s=ApiCharToFontIndex(*s);
		s++;
	}
}
#define SystemServer_WriteToScreen GfxApiWriteToConsole
// for compatibility until all demos are fixed
extern void GfxApiWriteToConsole(const char *txt, u8 *screen, u8 x, u8 y)
{
	int p = x + 16 * y;
	int l = strlen(txt);
	int i;
	for (i = 0; i < l; i++)screen[(i + p) & 0x7f] = ApiCharToFontIndex(txt[i]);
}
#ifdef ENABLE_LINEDRAWING
void _reorder_lines()
{
	u8 i;
	for(i=0;i<_gfx_linepos;i+=4)
	{
		if(_gfx_points_of_lines[i]>_gfx_points_of_lines[i+2])
		{
			swapu8(_gfx_points_of_lines[i],_gfx_points_of_lines[i+2]);
			swapu8(_gfx_points_of_lines[i+1],_gfx_points_of_lines[i+3]);
		}
	}
}
#endif

#pragma GCC push_options
#ifndef DISABLE_OFAST
#pragma GCC optimize ("Ofast")
#endif
static u8 os_gfx_read_bit (GfxApiCompressedLayer *g)
{
	u8 ret = pgm_read_byte(g->BytePos)&g->Bitpos;
	g->Bitpos <<= 1;
	if (!g->Bitpos)g->Bitpos = 1, g->BytePos++;
	return ret;
}

static u8 GfxApiReadSprite(GfxApiSprite *s)
{
	if(s->SkipCounter)
	{
		s->SkipCounter--;
		return 0;
	}
	u8 ret=pgm_read_byte(s->readpos_byte)&s->readpos_bit;
	s->readpos_bit<<=1;
	if(!s->readpos_bit)
	{
		s->SkipCounter=(64-8);
		s->readpos_bit=1;
		s->readpos_byte++;
		s->sprite_height--;
		if(!s->sprite_height)s->SkipCounter=0x7fff;
	}
	return ret;
}

static s8 os_gfx_layer_read_vlc(GfxApiCompressedLayer *g)
{
	u8 b = os_gfx_read_bit(g);
	if (!b)return 0;
	u8 sign = os_gfx_read_bit(g);

	s8 v = 0;
	
	if (!os_gfx_read_bit(g))goto finished;
	if (os_gfx_read_bit(g))v |= 4;

	if (!os_gfx_read_bit(g))goto finished;
	if (os_gfx_read_bit(g))v |= 8;

	if (!os_gfx_read_bit(g))goto finished;
	if (os_gfx_read_bit(g))
	{
		v |= 16;
	}
	else
	{
		v |= 32;
		goto finished;
	}
	if (os_gfx_read_bit(g))v |= 32;

	finished:
	v+=4;
	if (sign)return -v;
	return v;
}
 static s8 _GfxApiLayerGetNextByte(GfxApiCompressedLayer *g)
{
	s8 v = os_gfx_layer_read_vlc(g);
	if (v == 16*4)
	{
		g->SkipCounter = 0;
		char r;
		for (r = 0; r < 8; r++)
		{
			if (os_gfx_read_bit(g))
			g->SkipCounter |= 1 << r;
		}
		g->SkipCounter--;
		return 0;
	}
	if (v == -16*4)
	{
		g->SkipCounter = 0;
		int r;
		for (r = 0; r < 14; r++)
		{
			if (os_gfx_read_bit(g))
			g->SkipCounter |= 1 << r;
		}
		g->SkipCounter--;
		return 0;
	}
	return v;
}



#pragma GCC pop_options


static void os_gfx_start_display_transfer()
{
#ifndef ENABLE_WIRE

	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	os_i2c_write_byte(0x0);
	os_i2c_write_byte(0x0);
	os_i2c_write_byte(0x15);
	os_i2c_write_byte(0x0);os_i2c_write_byte(0x3f);
	os_i2c_write_byte(0x75);
	os_i2c_write_byte(0x0);os_i2c_write_byte(0x7f);

	xx
	os_i2c_stop();//*/
	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	os_i2c_write_byte(0x40);
#else
	Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
	Wire.write(0x0);

	Wire.write(0x15);
	Wire.write(0x0);Wire.write(0x3f);
	Wire.write(0x75);
	Wire.write(0x0);Wire.write(0x7f);


	Wire.endTransmission();
	Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
	Wire.write(0x40);
#endif
}
void os_init_ssd1306 (void)
{
	// brigher screen init:
#if 0 // ENABLE_DARKER_SCREEN	
	const u8 init1306[]={
	 0,0xe4,0xAE,0xD5, 0x80, 0xA8, 0x3F,0xD3, 0x0,0x40,0x8D, 0x14, 0x20, 0x01, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40,  0xA4,0xA6,0xAF
	};
#else
	// darker screen init:
	const u8 init1306_[] = {
		0x0,0x20, 1 /*vertical mode*/,0xB0,0xC8,0x00,0x10,0x40,0x81,0x0,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D, 0x14,0xAF
	};
	
	
	const u8 init1306[] = {
	 0,
	 0x0fd, 0x012,		/* unlock display, usually not required because the display is unlocked after reset */
	 0x0ae,		                /* display off */
	 //0x0a8, 0x03f,		/* multiplex ratio: 0x03f * 1/64 duty */
	 0x0a8, 0x05f,		/* multiplex ratio: 0x05f * 1/64 duty */
	 0x0a1, 0x000,		/* display start line */
	 //0x0a2, 0x04c,		/* display offset, shift mapping ram counter */
	 
	 0x0a2, 0x020,		/* display offset, shift mapping ram counter */
	 0x0a0, 0x051,		/* remap configuration */
	 
	 
	 0x0ab, 0x001,		/* Enable internal VDD regulator RESET */
	 //0x081, 0x070,		/* contrast, brightness, 0..128 */
	 0x081, 0x053,		/* contrast, brightness, 0..128 */
	 //0x0b1, 0x055,                    /* phase length */
	 0x0b1, 0x051,                    /* phase length */
	 //0x0b3, 0x091,		/* set display clock divide ratio/oscillator frequency set clock as 135 frames/sec */
	 0x0b3, 0x001,		/* set display clock divide ratio/oscillator frequency  */
	 
	 //? 0x0ad, 0x002,		/* master configuration: disable embedded DC-DC, enable internal VCOMH */
	 //? 0x086,				/* full current range 0x084, 0x085, 0x086 */
	 
	 0x0b9,				/* use linear lookup table */

	 //0x0bc, 0x010,                    /* pre-charge voltage level */
	 0x0bc, 0x008,                    /* pre-charge voltage level */
	 //0x0be, 0x01c,                     /* VCOMH voltage */
	 0x0be, 0x007,                     /* VCOMH voltage */
	 0x0b6, 0x001,		/* second precharge */
	 0x0d5, 0x062,		/* enable second precharge, internal vsl bit0 = 0 */


	 
	 0x0a4,				/* normal display mode */
	 0x15,0,64,0x75,0,128
	};
	
	
	
#endif	
	os_i2c_write(init_128x128, sizeof(init_128x128));
}



// set the brightness of the screen 0 to 15
void GfxApiSetBrightness (u8 brightness)
{
	brightness=brightness|(brightness<<4);
	u8 v1=brightness>>5;
	v1<<=4;
	u8 v2=brightness>>4;
	v2|=v2<<4;
	const u8 init1306[]={
		0,
		0xd9, v2,0xdb,v1,0x81,brightness
	};
	os_i2c_write(init1306, sizeof(init1306));
}

// set the brightness of the screen 0 to 15
void GfxApiSetFreq (u8 freq)
{	
	u16 a=(1+freq/2)<<4;
	a|=15-freq;
	
	const u8 init1306[]={
		0,
		0xd5,a 
	};
	os_i2c_write(init1306, sizeof(init1306));
}



void GfxApiSetDisplayEnable (u8 e)
{
	const u8 init1306[]={
		0,
		0x0ae +(e?1:0)
	};
	os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiFlipY(u8 flip_y)
{
	const u8 init1306[]={
		0,
		flip_y?	0xc0:0xc8
	};
	os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiSetInvert (u8 invert)
{
	const u8 init1306[]={
		0,
		0xa6+(invert?1:0)
	};
	os_i2c_write(init1306, sizeof(init1306));
}

#ifndef ENABLE_ARDUINO_SUPPORT
extern void MainTask();
int main()
{
	cli();
	I2C_DDR=0;
	I2C_PORT=255;  // input + pullup
	PRR=0b1100;   // disable timer, adc
	MCUCR|=0x80; // disable brownout
	ACSR|=0x80;
	
	os_i2c_init();
	os_init_ssd1306();
#ifdef ENABLE_ATTINY_POWER_MANAGER
	MCUSR=0;
	wdt_enable(WDTO_120MS);
	WDTCR|=1<<WDIE;             
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
#endif
	sei();
	MainTask();
}
#else
int streamgfx_init_arduino()
{
	os_i2c_init();
	os_init_ssd1306();
}
#endif

#ifdef ENABLE_ATTINY_POWER_MANAGER
ISR(WDT_vect)
{
	WDTCR |= 1<<WDIE;
}
u16 bat_volt=5200;
u8 low_power_screen_disable=0;
s8 _br=15;
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))

int _manage_battery() {
	static u8 phase=0;
	if(phase==1)
	{
		PRR&=~1; // enable adc....
		phase++;
		return bat_volt;
	}
	if(phase==2)
	{
		ADMUX =
		(0 << REFS1) | // Sets ref. voltage to VCC, bit 1
		(0 << REFS0) | // Sets ref. voltage to VCC, bit 0
		(1 << MUX3)  | // use Vbg as input, MUX bit 3
		(1 << MUX2)  | // use Vbg as input, MUX bit 2
		(0 << MUX1)  | // use Vbg as input, MUX bit 1
		(0 << MUX0);   // use Vbg as input, MUX bit 0
		ADCSRA =
		(1 << ADEN)  | // enable ADC
		(1 << ADPS2) | // set prescaler to 64, bit 2
		(1 << ADPS1) | // set prescaler to 64, bit 1
		(0 << ADPS0);  // set prescaler to 64, bit 0
		phase++;
		return bat_volt;
	}
	if(phase==3)
	{
		ADCSRA &= ~(1<<ADEN);
		ACSR |= (1<<ACD);
		ADCSRA |= (1<<ADEN);
		phase++;
		return bat_volt;
	}
	if(phase==4)
	{
		ADCSRA |= (1 << ADSC); // start ADC measurement
		while ( ADCSRA & (1 << ADSC) ); // wait till conversion complete
		uint16_t adc=ADC;
		// clear the ADIF bit by writing 1 to it
		ADCSRA|=(1<<ADIF);
		// disable the ADC
		ADCSRA &= ~(1<<ADEN);
		phase++;
		bat_volt=1000.0f*1024.0f*1.1f/(float)adc;
		return bat_volt;
	}
	PRR|=1;
	if(bat_volt<2000)
	{
		if(!low_power_screen_disable)
		{
			GfxApiSetDisplayEnable(0);

			const u8 disable_charge_pump[]={
				0,
				0x8d,
				0x10
			};
			os_i2c_write(disable_charge_pump, sizeof(disable_charge_pump));
			
			clock_prescale_set(clock_div_256);
		}
		low_power_screen_disable=10;
	}
	else
	{
		if(bat_volt<POWER_MANAGER_LOW_POWER)
		{
			
			if(bat_volt<POWER_MANAGER_LOW_POWER-50)
			_br-=1;
			if(_br<0)_br=0;	
		//	sleep_cpu();
		//	os_i2c_init();  // bring usi back....
		}
		else
		{	
			if(bat_volt>POWER_MANAGER_LOW_POWER+50)
			{
				_br++;
				if(_br>15)_br=15;
			}
		}
		_br=15;
		GfxApiSetBrightness(_br);

		if(low_power_screen_disable)
		{
			sleep_cpu();
			if(bat_volt>3300)
			{
				sleep_cpu();
				low_power_screen_disable--;
				if(!low_power_screen_disable)
				{
					const u8 reset[]={
						0,
						0xe4 // undocumented command, said to help startup if display is "confused"
					};
					clock_prescale_set(clock_div_1);

					_delay_ms(500);
					for (int i=0;i<18;i++)
						os_i2c_stop();
				
					os_init_ssd1306();
				}
			}
		}
	}
	phase++;
	if(phase==10)phase=0;
}
#endif