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


*****************If the Display does not work:
when using arduino try:

change
#define WIRE_SCREEN_ADDRESS 0x3C
to
#define WIRE_SCREEN_ADDRESS 0x3D


when using custom i2c change (in tiny_multi_os.h)

#define SSD1306_ADDRESS 0x78
to
#define SSD1306_ADDRESS 0x7A



Version History:



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

0.8.2		Support Video Codec, Support font replacement for Vector Quantization, demo: https://www.youtube.com/shorts/jIQZTjM_cL8

Description of the Demos


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


************** Defines to configure the Library:

#define ENABLE_LOW_QUALITY_HALFTONE   // turn on low quality halftoning, saves 64 bytes of ram, looks different, not allways worse

#define ENABLE_ARDUINO_SUPPORT   // <---enable support for Arduino

#define ENABLE_WIRE		// use Wire instead of Speed optimized I2C, the speed optimized I2C is only tested on the Attiny85

#define WIRE_SCREEN_ADDRESS 0x3C    // <----------------set the Display Adress for Wire

#define DISPLAYFUNC Display      // set the name of the function created by #include "displayfunc.h", displayfunc.h can be included multiple times to create more than one function with a different configuration

#define ENABLE_SPRITES // enable the display function to show sprites

#define ENABLE_LAYERS // enable rendering of compressed layers

#define NR_LAYERS 1   // configure the number of layers

#define SUBTRACT_LAYER0  // makes the first layer subtractive instead of additive

#define LAYERS_COLORKEY  // make the layers use colorkeying instead of beeing additive, zero is transparent (see boingball example)

#define ENABLE_SPRITES  // enable rendering of sprites, every sprite is 8 pixels high, x resolution is user defined

#define NR_SPRITES 0  // configure number of sprites


#define PIXEL_CALLBACK callbackfunctionname    // define a callback, this function will be called with the current x and y position on the screen, while the engine is rendering, it returns a 6 bit value (0-63) this value is the brightness of the pixel to be drawn

#define ENABLE_CONSOLE   		// enables the first console

#define ENABLE_SECOND_CONSOLE		// enables the second console (see zoom and scroll example), the second console has additional features, it can be zoomed and scrolled, it is slower than the first one

#define CONSOLE_LINE_START 0 // define starting row of the console on screen, 0-7, 0 results in the console starting at x,y=0,0, 1 results in x,y=0,8 
#define CONSOLE_LINE_END 7 // define the last row of the console on screen 0 is y=0, 1 is y=8 ...

#define SECOND_CONSOLE_LINE_START 0 // define starting row of the console on screen, 0-7, 0 results in the console starting at x,y=0,0, 1 results in x,y=0,8 
#define SECOND_CONSOLE_LINE_END 7 // define the last row of the console on screen 0 is y=0, 1 is y=8 ...

#define ENABLE_USERFONT 1   // add aditonal chars to the font, the library will include "userfont.h", this file contains the bitmaps of the new chars, binary in rows.





