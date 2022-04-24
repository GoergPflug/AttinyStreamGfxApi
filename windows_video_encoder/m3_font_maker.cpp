#define MAX_TILES 128


#undef UNICODE
#undef _UNICODE
#define _CRT_RAND_S
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <stdio.h>

#include <atlimage.h>
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

int img_xres = 400*3;
int img_yres = 400;

static unsigned char blue_noise[512 * 512];

unsigned short rnd()
{
	unsigned int r = 0;
	rand_s(&r);
	return r;
}

typedef struct vq
{
	int a[8 * 8];
	double sum[8 * 8];
	double error_cum;
	int usage;


	double distance(unsigned char *block)
	{
		double d = 0;
		int i;
		for (i = 0; i < 8 * 8; i++)d += (a[i] - block[i])*(a[i] - block[i]);
		return d;
	}
	void put(unsigned char *img, int xp, int yp, int xres,int idx)
	{
int		map[8 * 8] = {
	0,48,12,60, 3,51,15,63,
   32,16,44,28,35,19,47,31,
	8,56, 4,52,11,59, 7,55,
   40,24,36,20,43,27,39,23,
	2,50,14,62, 1,49,13,61,
   34,18,46,30,33,17,45,29,
   10,58, 6,54, 9,57, 5,53,
   42,26,38,22,41,25,37,21 }
;
		int map2[8*8] = {
 0, 32, 8, 40, 2, 34, 10, 42, /* 8x8 Bayer ordered dithering */
48, 16, 56, 24, 50, 18, 58, 26, /* pattern. Each input pixel */
12, 44, 4, 36, 14, 46, 6, 38, /* is scaled to the 0..63 range */
60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
 3, 35, 11, 43, 1, 33, 9, 41, /* to determine the action. */
51, 19, 59, 27, 49, 17, 57, 25,
15, 47, 7, 39, 13, 45, 5, 37,
63, 31, 55, 23, 61, 29, 53, 21 };
		int i = 0;
		int x, y;
		double err = 0;
		int r;
		int idx_x = idx & 0xf;
		int idx_y = idx >> 4;
		idx_x *= 8;
		idx_y *= 8;
		err = 0;
		for (i = 0; i < 64; i++)err += a[i];
		err /= 64;
		err = 0;
		int yy;
		if(0)
		for (yy = 0; yy < 8; yy+=2)
		{ 
			err = 0;
		for (int t=0;t<4;t++)
			for (y = yy; y < yy + 2; y++)

			{
				int xs = 0;
				int xe = 8;
				int xd = 1;
				if (y & 1)xs = 7, xe = -1, xd = -1;
				for (x = xs; x != xe; x += xd)
				{

					int c = a[x + y * 8];
					err += c;

					if (err > 127)err -= 255, c = 255; else c = 0;
							c = a[x + y * 8];

					img[(x + xp) * 3 + (y + yp) * xres * 3] = c,// a[i],
						img[(x + xp) * 3 + (y + yp) * xres * 3 + 1] = c,// a[i],
						img[(x + xp) * 3 + (y + yp) * xres * 3 + 2] = c;// a[i], i++;

				}
			}
		}
		i = 0;

if(1)
		for (y = 0; y < 8; y++)
			for (x = 0; x < 8; x++)
			{
				int c = a[i];
				if (c > blue_noise[x+idx_x+512*(y+idx_y)])c = 255; else c = 0;
//				c = a[i]/4;
			//	c = a[i];

	//			if (c > map2[x+8*y])c = 255; else c = 0;
				//if (c > (rand() & 0xff))err=255-c,c = 255; else err=c, c = 0;
				img[(x + xp) * 3 + (y + yp) * xres * 3] = c,// a[i],
				img[(x + xp) * 3 + (y + yp) * xres * 3 + 1] = c,// a[i],
				img[(x + xp) * 3 + (y + yp) * xres * 3 + 2] = c, i++;// a[i], i++;

			}
			
	}

	void dump(int idx,FILE *f)
	{
		int		map[8 * 8] = {
			0,48,12,60, 3,51,15,63,
		   32,16,44,28,35,19,47,31,
			8,56, 4,52,11,59, 7,55,
		   40,24,36,20,43,27,39,23,
			2,50,14,62, 1,49,13,61,
		   34,18,46,30,33,17,45,29,
		   10,58, 6,54, 9,57, 5,53,
		   42,26,38,22,41,25,37,21 }
		;
		int map2[8 * 8] = {
 0, 32, 8, 40, 2, 34, 10, 42, /* 8x8 Bayer ordered dithering */
48, 16, 56, 24, 50, 18, 58, 26, /* pattern. Each input pixel */
12, 44, 4, 36, 14, 46, 6, 38, /* is scaled to the 0..63 range */
60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
 3, 35, 11, 43, 1, 33, 9, 41, /* to determine the action. */
51, 19, 59, 27, 49, 17, 57, 25,
15, 47, 7, 39, 13, 45, 5, 37,
63, 31, 55, 23, 61, 29, 53, 21 };
		int i = 0;
		int x, y;
		double err = 0;
		int r;
		int idx_x = idx & 0xf;
		int idx_y = idx >> 4;
		idx_x *= 8;
		idx_y *= 8;
		err = 0;
		for (i = 0; i < 64; i++)err += a[i];
		err /= 64;
		err = 0;
		int yy;


		if (1)
			for (x = 0; x < 8; x++)
			{
				int d = 0;
				for (y = 0; y < 8; y++)
				{
					int c = a[x + y * 8];
					if (c > blue_noise[x + idx_x + 512 * (y + idx_y)])c = 255; else c = 0;
					//				c = a[i]/4;
								//	c = a[i];

						//			if (c > map2[x+8*y])c = 255; else c = 0;
									//if (c > (rand() & 0xff))err=255-c,c = 255; else err=c, c = 0;
				
					if (c)d |= 1<<y;
				}
				fprintf(f, "%d,", d);
			}
	}

	void add(unsigned char *block)
	{
		int i;
		for (i = 0; i < 8 * 8; i++)sum[i]+=block[i];
		usage++;

	}
	void update()
	{
		if (!usage)
		{
			clear();
			return;
		}
		for (int i = 0; i < 8 * 8; i++)a[i]=0.5+ sum[i]/usage,sum[i]=0;
		usage = 0;
	}
	void clear()
	{
		for (int i = 0; i < 8 * 8; i++)sum[i] = 0;
		usage = 0;
	}
	void randomize()
	{
		for (int i = 0; i < 8 * 8; i++)a[i] += rnd() % 3 - 1;
	}
};
vq v[256];

void get_block(unsigned char *img, int xres, int xp,int yp, unsigned char *out)
{
	int x, y;
	for(y=0;y<8;y++)
		for (x = 0; x < 8; x++)
		{
			*out = img[x + xp + (y + yp)*xres];
			out++;
		}
}


int vq_nr = 1;

int find_best(unsigned char *block, double &e)
{
	double err = v[0].distance(block);
	int best = 0;
	int i;

//	#pragma loop(hint_parallel(4))

	for (i = 0; i < vq_nr; i++)
	{

		double err2 = v[i].distance(block);
		if (err2 < err)err = err2, best = i;
	}
	e = err;
	return best;
}
void
display_img(unsigned char *img,int xr,int yr)
{
	HDC dc = GetDC(GetConsoleWindow());

	BITMAPINFO info;
	ZeroMemory(&info, sizeof(BITMAPINFO));
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biWidth = xr;
	info.bmiHeader.biHeight = -yr;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biSizeImage = 0;
	info.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(dc, 512 , 0, xr,  yr, 0, 0, xr, yr, img, &info, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(GetConsoleWindow(), dc);

}

int dump = 0;
static unsigned char * data;// [2024 * 1024 * 8];
static unsigned char v_out[1024 * 1024];
int v_out_pos = 0;

void write_bytes(FILE *f, unsigned char *b, int l)
{
	int i;
	for (i = 0; i < l; i++)fprintf(f, "%d,", b[i]);
}

void compress_video()
{
	int b = 128;
	FILE *f = fopen("vid.h", "wb");
	write_bytes(f, v_out, 128);
	for (int frame = 1; frame < img_yres / 64; frame++)
	{
		fprintf(f, "\r\n//Frame:%d bytes before:%d\r\n", frame,b);
		int i;
		int eq = 0;

		fprintf(f, "\r\n//");

		for (i = 0; i < 128; i++)fprintf(f, "%d ", v_out[i+frame*128]);
		fprintf(f, "\r\n");

		for (i = 0; i < 128; i++)
		{
			if (v_out[frame * 128 + i] == v_out[(frame - 1) * 128 + i])
			{
				//fprintf(f, "128,");
				eq++;
			}
			else
			{
				if (eq)
				{
					fprintf(f, "\r\n%d,/*skip:%d*/\r\n", eq+ 127,eq);
					eq = 0;
					b++;
				}
				fprintf(f, "%d,", v_out[frame*128+i]);
				b++;
			}
		}
		if (eq)
		{
			fprintf(f, "\r\n%d,/*skip:%d last*/\r\n", eq+127, eq);
			eq = 0;
			b++;
		}
	}
	fprintf(f, "//bytes:%d\r\n", b);
	fclose(f);
}

static unsigned char *d_out;// [1024 * 3 * 1024 * 8];
int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Video_Encode file.png");
		exit(0);
	}
	img = new CImage();
	int res = img->Load(argv[1]);
/*	if (!res)
	{
		printf("file not found\r\n");
		exit(0);
	}*/
	//int depth = img->GetBPP();
	img_xres = img->GetWidth();
	img_yres = img->GetHeight();
	printf("xres (should be 128)  %d yres %d", img_xres, img_yres);

	data = (unsigned char *)malloc(img_xres*img_yres);

	d_out = (unsigned char *)malloc(img_xres*img_yres*3);
	for (int x = 0; x < img_xres; x++)
		for (int y = 0; y < img_yres; y++)
		{
			COLORREF c = img->GetPixel(x, y);
			data[x + img_xres * y] = GetGrayscale(c);
		}



	FILE*	f = fopen("blue_noise512x512.raw", "rb");
	if (!f)
	{
		printf("blue noise texture missing\r\n");
		exit(0);
	}
	fread(blue_noise, 512, 512, f);

	int x,y,i,c = 2;
	FILE *f1=0,*f2=0;
	for (;;)
	{
		char tmp[256];
		if (dump)f1 = fopen("map.h", "wb");
		double e_a = 0;
		for (y = 0; y < img_yres; y += 8)
		for (x = 0; x < img_xres; x += 8)
			{
				double e = 0;
				unsigned char block[8 * 8];
				get_block(data, img_xres, x, y, block);
				int b = find_best(block,e);
				v[b].error_cum += e;
				//if((vq_nr==256)||((rnd()&0xff)!=0))
					v[b].add(block);
					e_a+= e;
					if (dump)
					{
						if (x == 0)
						{
							if ((y % 64) == 0)fprintf(f1, "\r\n//frame:%d\r\n", y / 64);
							fprintf(f1, "\r\n");
						}
						fprintf(f1, "%3d,", b);
						v_out[v_out_pos] = b;
						v_out_pos++;
					}
				v[b].put(d_out, x, y, img_xres,b);
			}

		if (dump)
		{
			f2 = fopen("charset.h", "wb");
			for (i = 0; i < MAX_TILES; i++)v[i].dump(i, f2);
			fclose(f2);
			fclose(f1);
		}

		double e_max = v[0].error_cum / v[0].usage;
		int worst = 0;

		for (i = 0; i < vq_nr; i++)if(v[i].usage)
			if (v[i].error_cum / v[i].usage > e_max)
			{
				e_max = v[i].error_cum / v[i].usage;
				worst = i;
			}
		static int ccc = 1000;
		display_img(d_out, img_xres, img_yres);
		if(vq_nr<MAX_TILES/2-1)
		for (i = vq_nr; i < vq_nr * 2; i++)v[i] = v[i - vq_nr];
		int unused = 0;
		for (i = 0; i < vq_nr; i++)
		{
			if((!unused)&&(v[i].usage < 1))//&&(!(rnd()&0xff)))
			{
				ccc--;
				if(ccc>0)
				c = 25;
				unused++;
				if ((rnd() & 0xf) == 0)worst = rnd() % vq_nr;
				v[i].clear();
				v[i] = v[worst];

				v[i].randomize();
				//	printf("%d,", v[i].usage);
			}
			else
			v[i].update();

		}
		printf("%d   %f         \r", unused,e_a);
	//	getchar();
		c--;
		static int full_c = 0;
		if (dump)
		{
			compress_video();
			exit(0);
		}
		if (!c)
		{
			//if(vq_nr==1) 
			if(vq_nr!=MAX_TILES)	
			for (i = vq_nr; i < vq_nr * 2; i++)v[i].randomize();// = v[i - vq_nr];

			c = 25;
			printf("\r\n%d %d\r\n", vq_nr,unused);
			if (vq_nr == MAX_TILES)full_c++;
			if (full_c == 10)dump = 1;
			if (vq_nr < MAX_TILES-1)vq_nr *= 2;
		}
	}
}
