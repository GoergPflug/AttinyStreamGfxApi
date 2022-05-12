#define F_CPU 16000000
#include <stdio.h>

// To get it running change the #include "/dev/...." to the place where you stored the header Files!

///////////////////////////////////////////////////////////////////////////////
// OS-Configuration////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define ENABLE_ATTINY_POWER_MANAGER
#define POWER_MANAGER_LOW_POWER 4200
#define ENABLE_POWER_MANGER_FADE
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
#define DISPLAYFUNC Display
#define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_END (SECOND_CONSOLE_LINE_START+(SECOND_CONSOLE_SIZE_Y*8))
#define SECOND_CONSOLE_LINE_LENGTH 17
//#define ENABLE_SPRITES
//#define PIXEL_CALLBACK
//#define ENABLE_LAYERS
#define ENABLE_CONSOLE
#define ENABLE_SECOND_CONSOLE
#define ENABLE_SECOND_CONSOLE_AFFINE
#define NR_LAYERS 1
#define NR_SPRITES 0
//#define SUBTRACT_LAYER0
#define CONSOLE_SIZE_Y 1
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START (0*8)

#define SECOND_CONSOLE_SIZE_Y 3
#define SECOND_CONSOLE_LINE_START (2*8)

////////
#define ENABLE_FONT_BASIC
//#define ENABLE_ARDUINO_SUPPORT
#define ENABLE_CONSOLE_BIT7_INVERT
//#define ENABLE_SECOND_CONSOLE_AFFINE
#include "/dev/tiny_multi_os.h"
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Build Render Function from Defines
#include "/dev/displayfunc.h"
/////////////////////////////////////////////////////////////////////

#include <util/delay.h>

extern void
#ifdef ENABLE_MULTITASK
__attribute__((OS_task))
#endif
MainTask()

{
	static unsigned char console[16*8];
	static unsigned char console2[128];
	memset(console2,ApiCharToFontIndex(' '),sizeof(console2));
	SystemServer_WriteToScreen("Battery Voltage",console,0,0);

	int i;for(i=0x20;i<128;i++)console[i-0x20+16]=i-0x20;
	unsigned int ttt=0,ttt2=0;
	GfxApiSetBrightness(15);
	for(;;)
	{
		char tmp[16];
		static char toggle=0;
		//int bat_volt=getVcc();
		toggle^=1;
		if(toggle)
			sprintf(tmp,"%d  ",bat_volt);
		else
			sprintf(tmp,"%d !",bat_volt);
		
		
		SystemServer_WriteToScreen(tmp,console2,2,1);		
		Display(0, 0, 0,console,console2+17,128,128,0,0,0,-8 );  // display second console zoom 2x text slightly downwards to the right
	//	m_vol();
	}
}
