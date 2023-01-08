// ConsoleApplication9_stretchdibbits.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#undef UNICODE
#undef _UNICODE
char filename[256];

unsigned char image[64 * 128];

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <windows.h>
#include <math.h>
#include <atlimage.h>



int bitpos = 0;

int negate = 0;

int zero_min = 17;
static unsigned char bit_out[0xffff];

int bytes = 0;

unsigned char byte_encoded[0xffff];
int byte_codepos;
FILE *fout;
void emit(int v)
{
	byte_encoded[byte_codepos] = v;
	byte_codepos++;
	printf("%0x.", v);
	fprintf(fout, "%d,", v);
	bytes++;
}
#define u8 unsigned char
#define u16 unsigned short
typedef struct GfxApiCompressedLayer
{
	u16 SkipCounter;
	u8 PixelValue;
	u8* BytePos;
} GfxApiCompressedLayer;

unsigned char LayerGetNextByte(GfxApiCompressedLayer *l)
{
	if (l->SkipCounter)
	{
		l->SkipCounter--;
		return l->PixelValue;
	}
	u8 r = *l->BytePos;
	l->BytePos++;
	u8 color = (r >> 5);
	l->PixelValue = color;
	u8 rept = r & 31;
	l->SkipCounter = rept;
	if (rept != 31)return color;
	for (;;)
	{
		u8 add = *l->BytePos;
		l->BytePos++;
		l->SkipCounter += add;
		if (add != 255)return color;
	}
}

void validate()
{
	GfxApiCompressedLayer l;
	l.BytePos = byte_encoded;
	l.SkipCounter = 0;
	int i;
	for (i = 0; i < 64 * 128; i++)
	{
		printf("(%d %d %d)", image[i] >> 5, LayerGetNextByte(&l),l.SkipCounter);
	}
}


void encode_image_bytewise()
{
	int i;
	int color_shr = 5;

	for (i = 0; i < 128 * 64;)
	{
		int len;
		int color = image[i] >> color_shr;
		for (len = i; len < 128 * 64; len++)
		{
			if ((image[len] >> color_shr) != color)break;
		}
		len -= i;
		printf("(%d)", len);

		int o = len-1;
		int o2 = o;

		if (o2 > 30)o2 = 31;
		emit((color << color_shr) | o2);
		if (o2 == 31)
		{
			o -= 31;
			for (;;)
			{
				if (o > 254)
				{
					emit(255);
					o -= 255;
				}
				else
				{
					emit(o);
					break;
				}
			}
		}
		i += len;
	}

	printf("predict:%d\r\n", bytes);
}



void write_bit(int b)
{
	int p = bitpos >> 3;
	int bit = bitpos & 7;
	if (b)	bit_out[p] |= 1 << bit;
	bitpos++;
}

unsigned char read_bitpos = 1;
unsigned int read_bytepos = 0;
unsigned char read_bit()
{
	unsigned char ret = 0;
	if (bit_out[read_bytepos] & read_bitpos)
	{
		ret = 1;
	}
	read_bitpos <<= 1;
	if (!read_bitpos)read_bitpos = 1, read_bytepos++;
	return ret;
}

signed char read_vlc2()
{
	unsigned char b = read_bit();
	if (!b)return 0;
	unsigned char sign = read_bit();

	signed char v = 0;

	if (!read_bit())goto finished;
	if (read_bit())v |= 1;

	if (!read_bit())goto finished;
	if (read_bit())v |= 2;

	if (!read_bit())goto finished;
	if (read_bit())
	{
		v |= 4;
	}
	else
	{
		v |= 8;
		goto finished;
	}
	//if (!read_bit())goto finished;
	if (read_bit())v |= 8;

finished:
	v++;
	if (sign)return -v;
	return v;
}
int zeros = 0;
signed char read_vlc(int i)
{
	if (zeros)
	{
		zeros--;
		return 0;
	}
	signed char v = read_vlc2();
	if (v == 16)
	{
		zeros = 0;
		char r;
		for (r = 0; r < 8; r++)
		{
			if (read_bit())
				zeros |= 1 << r;
			//printf("r:%d", zeros & 1);
		}
		//		printf("dec zeros:%d %d\r\n", i, zeros);
			//	getchar();
		zeros--;
		return 0;
	}
	if (v == -16)
	{
		zeros = 0;
		int r;
		for (r = 0; r < 14; r++)
		{
			int rr = read_bit();
			if (rr)
				zeros |= 1 << r;
		}

		zeros--;
		return 0;
	}
	return v;
}


int deltapos = 0;
int deltatab[256 * 256];

void vlc_signed(int v)
{
	if (negate)v = -v;
	deltatab[deltapos] = v;
	deltapos++;
	if (v == 0)
	{
		write_bit(0);
		return;
	}
	write_bit(1);
	int s = 0;
	if (v < 0)s = 1, v = -v;
	write_bit(s);
	v--;

	int r = 0;

	write_bit(v);
	if (!v)return;
	write_bit(v & 1);
	v >>= 1;

	write_bit(v);
	if (!v)return;
	write_bit(v & 1);
	v >>= 1;

	write_bit(v);
	if (!v)return;
	write_bit(v & 1);
	if (!(v & 1))return;   // das letzte bit muss 1 sein, sonst wären wir nicht hier.
	v >>= 1;

	write_bit(v);
}
void encode_image(int tolerance, int shr)
{
	int v = 0, i;
	for (i = 0; i < 64 * 128; i++)
	{
		int p = image[i] >> (4 + shr);
		int delta = p - v;
		if ((p != 0) && (abs(delta) <= tolerance))delta = 0;
		deltatab[i] = delta;

		v += delta;// = p;
	};

	for (i = 0; i < 64 * 128;)
	{
		int zeros = 0;
		int r;
		for (r = i; r < 64 * 128; r++)
		{
			if (deltatab[r] != 0)break;
			zeros++;
			//if (zeros > 254)break;
		}
		if (zeros > zero_min)
		{
			if (zeros < 256)
			{
				vlc_signed(16);
				i += zeros;
				for (r = 0; r < 8; r++)
				{
					write_bit(zeros & (1 << r));
				}
			}
			else
			{
				vlc_signed(-16);
				i += zeros;
				for (r = 0; r < 14; r++)
				{
					write_bit(zeros & (1 << r));
				}
			}


		}
		else
		{
			vlc_signed(deltatab[i]);
			i++;
		}
	}


	bitpos >>= 3;
	bitpos++;

	printf("\r\ncompressed output:%d bytes\r\n", bitpos);

	FILE *f = fopen(filename, "wb");
	for (i = 0; i < bitpos; i++)fprintf(f, "%d,", bit_out[i]);
	fclose(f);
}
/* test decoding*/
/*
void decode_image()
{
	int i;
	int last = 0;
	for (i = 0; i < 128 * 64; i++)
	{

		last += read_vlc(i);
		printf("(%d,%d)", image[i] >> 4, last);
		if ((image[i] >> 4) != last)
		{
			printf("flasch!\r\n");
			getchar();
		}
	}
}
*/
CImage *img;
unsigned char GetGrayscale(COLORREF cr)
{
	unsigned char byColor;

	byColor =
		(GetRValue(cr) * 0.30) +
		(GetGValue(cr) * 0.59) +
		(GetBValue(cr) * 0.11);
	return byColor;
}
int main(int argc, char *argv[])
{
	img = new CImage();


	if (argc != 6) {
		printf("usage AttinyImageCompress inputfile.png /.jpg /.gif /.bmp tolerance rightshift negation maxZero\r\n");
		printf("inputfile  : raw image data 8bit, 64x128 resolution\r\n");
		printf("tolerance  : accept an error of n in the outputdata, increases compression, decrease quality, use: 0\r\n");
		printf("rightshift : shift image n bits to the right, use: 0\r\n");
		printf("negation   : negate output delta, untested, use: 0\r\n");
		printf("maxZero    : minimum Numbers of Zeros to join to a block, use: 17 (!) ");
		printf("Example: AttinyImageCompress myImage.png 0 0 0 17\r\n");
		exit(0);
	}
	sprintf(filename, argv[1]);
	/*
	FILE *ff = fopen(filename, "rb");
	if (!ff)
	{
		printf("File not Found!\r\n");
		return 0;
	}
	*/

	int res = img->Load(filename);
	if (img->IsNull())
	{
		printf("could not load image!\r\n");
		exit(0);
	}

	int depth = img->GetBPP();
	int xres = img->GetWidth();
	int yres = img->GetHeight();
	if (xres != 64)
	{
		printf("image resolution must be 64 x 128 but it is %d x %d\r\n", xres, yres);
		exit(0);
	}
	if (yres != 128)
	{
		printf("image resolution must be 64 x 128 but it is %d x %d\r\n", xres, yres);
		exit(0);
	}

	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 128; y++)
		{
			COLORREF c = img->GetPixel(x, y);
			image[x + 64 * y] = GetGrayscale(c);
		}


	//fread(image, 64, 128, ff);

	sprintf(filename, "%s.h", argv[1]);
	negate = atoi(argv[4]);
	zero_min = atoi(argv[5]);
	//encode_image(atoi(argv[2]), atoi(argv[3]));
	fout= fopen(filename, "wb");

	encode_image_bytewise();
	fclose(fout);
	validate();
	//	getchar();
	//	printf("decode!\r\n");
	//	decode_image();
	//	getchar();
}
