# AttinyStreamGfxApi
Fast Graphics Library for Attiny85 and Aduino with SSD1306 over I2c, it also works on Arduino Boards. license: bsd license.

this is a fast datastream-based graphics library for the Attiny85, Version 0.8, and some examples how to use it.

it is primarly based on macros and data structures for layers, sprites, two consoles, the second console has additional features, it can be zoomed and scrolled. Almost no buffers are used, the library works by directly synthesizing the I2C "Pixel Stream", Resource usage is low, for example the Diagram example uses 29 Bytes and additional 68 bytes stack max. the Raxcaster uses 154 Bytes (about 90 bytes of which are on the stack), most demos should in fact run on an attiny45 (but have not been tested)

layers can be converted from png, jpeg, bmp, gif images data using a windows tool.

the library internaly works with 6 Bit greyscale which is converted to black and white using halftoning.

there are examples for 3D Rendering, text rendering and "calculated" graphics effects

for examples what can be done see

https://www.youtube.com/watch?v=WNJQXsJqSbM&t=20s

and

https://www.youtube.com/channel/UC5VMDSmyEacCnnA2sUn5uhw/videos


If the Display does not work:
=============================
when using arduino try:

change
#define WIRE_SCREEN_ADDRESS 0x3C
to
#define WIRE_SCREEN_ADDRESS 0x3D


when using custom i2c change (in tiny_multi_os.h)

#define SSD1306_ADDRESS 0x78
to
#define SSD1306_ADDRESS 0x7A

Note: SH1106 is not supported, the library depends on the vertical display mode of the SSD1306

Version History:
================


0.2		Multiple Consoles second Console with Zoom and Softscroll

0.2.1		ASCII Font Support
		
.		define: ENABLE_FONT_BASIC
		
.		"Low Quality" Halftoning, saves 64 bytes, faster, for some use cases "low quality" halftoning might even look better
		
.		define: ENABLE_LOW_QUALITY_HALFTONE
		
.		Halftoning gets disabled Automatically when not using layers or pixel callback, faster, saves 64 byte

0.3		Support C++, Suppport compiling on the Arduino IDE, example C++ Application for Arduino.

0.4		Prelimary Support for Arduino-Wire, performance worse than on attiny85

0.5-0.7		internal

0.8		Interleaved I2C, Faster I2C on Attiny, by mixing I2C-transfers with the rendercode, no more waiting for I2C on attiny, 10% faster

.		New integrated Line drawing Logic, Line based 3D 500% faster
	
.		Helper Functions for calculation of offsets
		
.		API-Cleanup: Sprite struct changed
		
.		New Api Functions for filled triangles and lines, adapted samples
		
.		New Example for filled 3D, see: https://www.youtube.com/shorts/ITSJokLk3jE
		
.		Affine Transformation "Mode7" for secondary console allows rotation and shearing see: https://www.youtube.com/shorts/MPjAXqZMenM

0.8.1		Support Clipping for Lines and Triangles, API-Change: Center of Screen is at 128,128 instead of 64,32 

0.8.2		Support Video Codec, Support font replacement for Vector Quantization, demo: https://www.youtube.com/shorts/qawMsh1IuQk
		and https://www.youtube.com/shorts/jIQZTjM_cL8
		
0.8.3		Support Chunked video encoding -> Videos >32KB for Arduino, support reading .AVI and .MP4 instead of large png. Added Conversion of the "Bad Apple Demo", working on Arduinos with 256kb Flash. see https://www.youtube.com/watch?v=pep0TOAl7SM Bad Apple Demo in Wokwi Emulator: https://wokwi.com/projects/329831079332545106

0.9		Support drawing Circles,  demo: https://www.youtube.com/shorts/0RNVlqsG6Ww bugfix removes flicker for filled polygon, Power Manager for Attiny reduces frame rate if power drops, fades screen, then disables the Display, reenables display if power back.
Description of the Demos
========================

#1 Diagram

In this demostration we draw 2 layers one layer is calculated, the diagram, another layer is blended over it, these layers each have brightness values from 0 to 63 are added together and then converted with error diffusion dithering to black and white


#2 Alien

This example shows subtractive blending, and text rendering, 3 layers are drawn, a text layer, above it a calculated layer with a graphic effect, and an image layer, the image layer is subtracted from the layer below it.


#3 Flexgrid

In the Flexgrid example, custom fonts are shown, 6 characters are added to the system font and with their help an animation is drawn on the console layer, on top of it there is a graphic layer


#4 Will it Bend

The "Will it Bend" example shows a classic graphics effect, a bending cuboid is drawn using the calculation plane alone.




#5 Oszilloskop

The "Oscilloscope" example shows the creation of user interfaces. A simulated oscilloscope with a mouse pointer is displayed. A text layer is output below it.




#6 Alleycat

"Alleycat" demonstrates the use of consoles and animations by switching layers. It is a "Hello World" example.


#7 Plasmastern

"Plasma star" shows fading effects and a calculated plasma graphic effect.


#8 Spaceship

"Spaceship" demonstrates additive blending and noise through random numbers.

#9 Wobble

"Wobble" shows the manipulation of layers on-the-fly. During decoding of the layer, a sine wave is superimposed over its vertical position.

#10 Origin 

"Origin" is a classic Copper effect. A graphic is output and manipulated column by column. This results in a fullscreen animation. In addition, a text layer is displayed.

#11 Face Value 

"Face Value" shows a line-by-line graphic effect.




#12 Boing Ball

"Boing Ball" demonstrates the shifting of layers and the use of color keying.

#13 3D Engine

The "3D Engine" shows a rotating cube. Primarily the drawing of lines is demonstrated.


#14 Crossfade

"Crossfade" reads data directly from layers to implement a crossfade between two images.




#15 Raycaster

"Raycaster" is a port of a simple raycasting engine for the PC. An engine of this type was also used in the original "Wolfenstein 3D".


Defines to configure the Library:
=================================

#define ENABLE_LOW_QUALITY_HALFTONE   // turn on low quality halftoning, saves 64 bytes of ram, looks different, not allways worse

#define ENABLE_ARDUINO_SUPPORT   // <---enable support for Arduino

#define ENABLE_WIRE		// use Wire instead of Speed optimized I2C, the speed optimized I2C is only tested on the Attiny85

#define WIRE_SCREEN_ADDRESS 0x3C    // <----------------set the Display Adress for Wire

#define DISPLAYFUNC Display      // set the name of the function created by #include "displayfunc.h", displayfunc.h can be included multiple times to create more than one function with a different configuration. 

#define ENABLE_SPRITES // enable the display function to show sprites

#define ENABLE_LAYERS // enable rendering of compressed layers

#define NR_LAYERS 1   // configure the number of layers

#define SUBTRACT_LAYER0  // makes the first layer subtractive instead of additive

#define LAYERS_COLORKEY  // make the layers use colorkeying instead of beeing additive, zero is transparent (see boingball example)

#define ENABLE_SPRITES  // enable rendering of sprites, every sprite is 8 pixels high, x resolution is user defined

#define NR_SPRITES 0  // configure number of sprites

#define ENABLE_LINES // Enable Line drawing API (see 3d Cube example how to use)

#define ENABLE_TRIANGLES // Enable Triangel drawing API

#define NR_LINES nr // Reserve Memory for storage of Lines

#define NR_TRIANGLES nr // reserve Memory for Triangle Points, "color", and 1 byte per Triangle which can be used for Dept Sorting

#define PIXEL_CALLBACK callbackfunctionname    // define a callback, this function will be called with the current x and y position on the screen, while the engine is rendering, it returns a 6 bit value (0-63) this value is the brightness of the pixel to be drawn. The Callback function will be called for every pixel, first for x,y=(0,0) then for (0,1) to (0,63) then x will increase to 1, so it will be called for x,y=(1,0) and so on.

#define ENABLE_CONSOLE   		// enables the first console

#define ENABLE_SECOND_CONSOLE		// enables the second console (see zoom and scroll example), the second console has additional features, it can be zoomed and scrolled, it is slower than the first one

#define CONSOLE_LINE_START 0 // define starting row of the console on screen, 0-7, 0 results in the console starting at x,y=0,0, 1 results in x,y=0,8 

#define CONSOLE_LINE_END 7 // define the last row of the console on screen 0 is y=0, 1 is y=8 ...

#define SECOND_CONSOLE_LINE_START 0 // define starting row of the console on screen, 0-7, 0 results in the console starting at x,y=0,0, 1 results in x,y=0,8 

#define SECOND_CONSOLE_LINE_END 7 // define the last row of the console on screen 0 is y=0, 1 is y=8 ...

#define ENABLE_SECOND_CONSOLE_AFFINE  // Enable affine transformations on the second console, this can be used to rotate or shear the console, see console rotation example for usage

#define ENABLE_USERFONT // add aditonal chars to the font, the library will include "userfont.h", this file contains the bitmaps of the new chars, binary in rows.

#define REPLACE_FONT "myfontinclude.h" // completly replace the built in Font with something else, can be used to implement a VQ-Video Codec. 

#define ENABLE_DARKER_SCREEN // alternative Initalization to make the Display less bright

#ifdef ENABLE_FONT_BASIC // Support full ASCII Charset on Console

Display Function
=================

Some of the Defines will change the API of the Display Function, and add aditional Parameters:

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

GFX-API Functions
============

Most of the functionallity is used by defining datastructures, Arrays of Char for Consoles, and Structs for Layers and Sprites and then passing these arrays/arrays of structs into the custom display function this function is generated by setting the defines to configure it and afterwards including "displayfunc.h", "displayfunc.h" can be included multiple times to generate several display functions which implement different functionality  , but there are also some API Functions:

u8 ApiCharToFontIndex(u8 c);  <----- convert a ascci Char to a font index, use this to write text into the console Arrays

extern void GfxApiWriteToConsole(const char *txt, u8 *screen, u8 x, u8 y)  <----- write a string to a console Array

GfxApiLayerGetNextByte (GfxApiCompressedLayer *g)  <---- decompress Layer data, this function is normally called by the displayfunction and not by the user application, every time it is called it will extract one pixel from the layer.it can be used to build custom layer functionallity by calling it in the Pixel Callback, see wobble for an example

static inline GfxApiPosition(unsigned char x, unsigned char y)  <---- Helper Function to Calculate a Position on the Screen from X,Y Coordinate, usefull for placing Sprites and Layers

static void GfxApiBeginLines()  <---- Start storage of Line Points

static void GfxApiStoreLinePoint(unsigned char x1, unsigned char y1)  <------ Store a point of a line, call this Function 2 times to draw a line, GfxApiBeginLines must be called before the first call to this function, but only ONCE per Frame

***Example: Draw a Line from 10,10 to 20,30 

	GfxApiStoreLinePoint (10,10);

	GfxApiStoreLinePoint (20,30);

***All Lines must be stored before calling the Triangle API

static void GfxApiBeginTriangles() <---- Start storage of Triangle Coordinates and Color & Depth Information

static void GfxApiStoreTrianglePoint(unsigned char x1, unsigned char y1)  <---- Store A Triangle Point
***Example:

Draw a Triangle from 10,10 to 0,20 to 30,30, completly filled, depth 20, also see the Filled 3d Example, sorting must be done by the user application, if you want soring see filled 3d example and look for "gnomesort"

	GfxApiStoreTrianglePoint(10,10);  // Point A

	GfxApiStoreTrianglePoint(0,20);   // Point B

	GfxApiStoreTrianglePoint(30,30);  // Point C

	GfxApiStoreTrianglePoint(0xff,20); // 0xff is the Pattern to fill the triangle, 20 is a additional byte which can be used for depth sorting.

void GfxApiSetBrightness (u8 brightness)
set the brightness of the screen 0 to 15







