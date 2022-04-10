#pragma GCC push_options
#pragma GCC optimize ("Ofast")

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
#endif

static void DISPLAYFUNC (
	GfxApiCompressedLayer * layers, 
	u8 fade, 
	GfxApiSprite * Sprites, 
	u8 * screen
#ifdef ENABLE_SECOND_CONSOLE
	,u8 * screen2, 
	u16 zoomX,
	u16 zoomY,
	u16 scrollX,
	u16 scrollY
#endif
	)
{
#ifdef ENABLE_WIRE
	u8 wire_cnt=0;
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
	s16 layersum=0;
#endif

#ifdef ENABLE_SECOND_CONSOLE
	u16 second_console_x=scrollX;
#endif
	for (x_pos_screen = 0; x_pos_screen < 128; x_pos_screen++
#ifdef ENABLE_SECOND_CONSOLE
	,second_console_x+=zoomX
#endif
	)
	{
#ifdef ENABLE_CONSOLE
		u8 charpos = x_pos_screen >> 3;
#endif
#ifdef ENABLE_SECOND_CONSOLE
		u8 charpos_second_con = (second_console_x>>11);
		u16 second_console_y=scrollY;
#endif
		u8 or_bit=1;
		s8 propagte_error=0;
		u8 block_8_px=0;
		u8 y_pos_screen;
		
		for (y_pos_screen = 0; y_pos_screen < 64; y_pos_screen++
#ifdef ENABLE_SECOND_CONSOLE
		,second_console_y+=zoomY
#endif
		
		)
		{
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
			
#ifdef ENABLE_SPRITES
			if(!sprites_constant_counter)
			{
				u8 i;
				for(i=0;i<NR_SPRITES;i++)
				if(GfxApiReadSprite(&Sprites[i]))block_8_px|=or_bit;
			}else sprites_constant_counter--;
#endif
			
#ifdef ENABLE_SECOND_CONSOLE
			if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen>>8)<SECOND_CONSOLE_LINE_END))
			{
				u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8)-SECOND_CONSOLE_LINE_START)>>3)]);
				const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x>>8) & 7)]);
				if(font_block8 & (1<<((second_console_y>>8)&7)))
					block_8_px|=or_bit;
			}
			#endif

			
#ifndef DISABLE_HALFTONE			
			int s=layersum;
			if(s<0)s=0;
			if(s>63)s=63;
#ifndef ENABLE_LOW_QUALITY_HALFTONE			
			propagte_error+=s+error_right[y_pos_screen];
#else
			propagte_error+=s;
#endif
			if (propagte_error > 31)block_8_px |= or_bit, propagte_error-=63;// = vpix - 255;
#ifndef ENABLE_LOW_QUALITY_HALFTONE
			propagte_error/=2;
			error_right[y_pos_screen]=propagte_error;
#endif
#endif			
			or_bit<<=1;
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
				
				
				
				
				os_i2c_write_byte(block_8_px);

				block_8_px = 0;
				#ifdef ENABLE_LAYERS
				if(!layers_constant_counter)
				{
					u16 min=0xffff;
					u8 i;
					for(i=0;i<NR_LAYERS;i++)
					{
						if(layers[i].SkipCounter<min)min=layers[i].SkipCounter;
					}
					if(min!=0xffff)
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
					u16 min=0xffff;
					u8 i;
					for(i=0;i<NR_SPRITES;i++)
					{
						if(Sprites[i].zeros<min)min=Sprites[i].zeros;
					}
					if(min!=0xffff)
					{
						sprites_constant_counter=min;
						for(i=0;i<NR_SPRITES;i++)
						Sprites[i].zeros-=sprites_constant_counter;
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
	os_i2c_stop();
}

#ifdef DISABLE_HALFTONE
#undef DISABLE_HALFTONE
#endif
#pragma GCC pop_options
