/* CPKI AttinyGfxApi & TinyMultiOs, Preview Version 0.2

see

https://www.youtube.com/watch?v=WNJQXsJqSbM


Copyright (c) 2002
Görg Pflug & CPKI Gmbh, www.cpki.de . All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the University of California, Berkeley and its contributors.”
Neither the name of the University nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE Görg Pflug, CPKI Gmbh AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*/


#define s8 signed char
#define u8 unsigned char
#define s16 signed int
#define u16 unsigned int

#define SSD1306_ADDRESS 0x78

#define PIN_SDA  0
#define PIN_SCL 2

void ApiIntToHex(u16 in, u8 *out);
u8 ApiCharToFontIndex(u8 c);

typedef struct GfxApiSprite
{
	u16 zeros;
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
static inline void os_i2c_sda_low (void);
static inline void os_i2c_scl_release (void);
void os_i2c_init (void);
static void os_i2c_write (const u8 *buf, u8 len);

#ifdef ENABLE_CONSOLE
__flash const u8 os_font[] = {
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
#ifdef ENABLE_USERFONT
#include "userfont.h"
#endif
};
#endif

static inline void os_i2c_scl_release (void)
{
	PORTB |= _BV(PIN_SCL);
}

static inline void os_i2c_scl_low (void)
{
	PORTB &= ~_BV(PIN_SCL);
}

static inline void os_i2c_sda_release (void)
{
	USIDR |= 0x80;
}

static inline void os_i2c_sda_low (void)
{
	USIDR &= ~0x80;
}

static void os_i2c_start (void)
{
	os_i2c_scl_release();
	os_i2c_sda_low();
	os_i2c_scl_low();
	os_i2c_sda_release();

	USISR = _BV(USISIF);
}

static void os_i2c_stop (void)
{
	os_i2c_sda_low();
	os_i2c_scl_release();
	os_i2c_sda_release();
	USISR = _BV(USIPF);
}

static void os_i2c_write_byte (u8 byte)
{
	USIDR = byte;
	USISR = 7;
	do {
		os_i2c_scl_release();
		os_i2c_scl_low();
		USICR |= _BV(USICLK);
	}
	while (USISR & 0x0F);
	os_i2c_sda_release();
}

void os_i2c_init (void)
{
	PORTB = _BV(PIN_SDA) | _BV(PIN_SCL);
	DDRB  = _BV(PIN_SDA) | _BV(PIN_SCL);
	USICR = _BV(USIWM1) | _BV(USICLK);
	os_i2c_scl_release();
	os_i2c_sda_release();
}

static void os_i2c_write (const u8 *buf, u8 len)
{
	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	while (len--)
		os_i2c_write_byte(*buf++);
	os_i2c_stop();
}
/*
FUSES =
{
	.low = LFUSE_DEFAULT,
	.high = HFUSE_DEFAULT,//(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE & FUSE_SPIEN ),
	.extended = FUSE_SELFPRGEN,
};
*/
__flash const u8 os_conv_hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void ApiIntToHex(u16 in, u8 *out)
{
	out[0] = os_conv_hex[(in >> 12) & 0xf];
	out[1] = os_conv_hex[(in >> 8) & 0xf];
	out[2] = os_conv_hex[(in >> 4) & 0xf];
	out[3] = os_conv_hex[in & 0xf];
}

u8 ApiCharToFontIndex(u8 c)
{
	if (c >= 'a' && c <= 'z')return 11 + c - 'a';
	if (c >= 'A' && c <= 'Z')return 11 + c - 'A';
	if (c >= '0' && c <= '9')return 1 + c - '0';
	return 0;
}

extern void SystemServer_WriteToScreen(u8 *txt, u8 *screen, u8 x, u8 y)
{
	int p = x + 16 * y;
	int l = strlen(txt);
	int i;
	for (i = 0; i < l; i++)screen[(i + p) & 0x7f] = ApiCharToFontIndex(txt[i]);
}

#pragma GCC push_options
#pragma GCC optimize ("Ofast")
static u8 os_gfx_read_bit(GfxApiCompressedLayer *g)
{
	u8 ret = pgm_read_byte(g->BytePos)&g->Bitpos;
	g->Bitpos <<= 1;
	if (!g->Bitpos)g->Bitpos = 1, g->BytePos++;
	return ret;
}

static u8 GfxApiReadSprite(GfxApiSprite *s)
{
	if(s->zeros)
	{
		s->zeros--;
		return 0;
	}
	u8 ret=pgm_read_byte(s->readpos_byte)&s->readpos_bit;
	s->readpos_bit<<=1;
	if(!s->readpos_bit)
	{
		s->zeros=(64-8);
		s->readpos_bit=1;
		s->readpos_byte++;
		s->sprite_height--;
		if(!s->sprite_height)s->zeros=0x7fff;
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
	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	os_i2c_write_byte(0x0);
	os_i2c_write_byte(0xb0);
	os_i2c_write_byte(0x21);
	os_i2c_write_byte(0x0);
	os_i2c_write_byte(0x7f);
	os_i2c_stop();
	os_i2c_start();
	os_i2c_write_byte(SSD1306_ADDRESS);
	os_i2c_write_byte(0x40);
}

void os_init_ssd1306 (void)
{
	const u8 init1306[] = {
		0x0,0x20, 1 /*vertical mode*/,0xB0,0xC8,0x00,0x10,0x40,0x81,0x0,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D, 0x14,0xAF
	};
	os_i2c_write(init1306, sizeof(init1306));
}

extern void MainTask();


int main()
{
	cli();
	os_i2c_init();
	os_init_ssd1306();
	MainTask();
}
