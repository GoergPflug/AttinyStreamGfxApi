# AttinyStreamGfxApi
Fast Graphics Library for Attiny85 and SSD1306 over I2c

this is a fast datastream-based graphics library for the Attiny85, preview version 0.2, and some examples how to use it.

it is primarly based on macros and data structures for layers, sprites, two consoles, the second console has additional features, it can be zoomed and scrolled.

layers can be converted from raw image data using a windows tool.

the library internaly works with 6 Bit greyscale which is converted to black and white using halftoning.

there are examples for 3D Rendering, text rendering and "calculated" graphics effects

for examples what can be done see

https://www.youtube.com/watch?v=WNJQXsJqSbM&t=20s

and

https://www.youtube.com/channel/UC5VMDSmyEacCnnA2sUn5uhw/videos

Version History:



0.2		Multiple Consoles second Console with Zoom and Softscroll

0.2.1		
		ASCII Font Support
		define: ENABLE_FONT_BASIC

		"Low Quality" Halftoning, saves 64 bytes, faster, for some use cases "low quality" halftoning might even look better
		define: ENABLE_LOW_QUALITY_HALFTONE
		
		Halftoning gets disabled Automatically when not using layers or pixel callback, faster, saves 64 byte

0.3		Support C++, Suppport compiling on the Arduino IDE, example C++ Application for Arduino.

0.4		Prelimary Support for Arduino-Wire, performance worse than on attiny85

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

