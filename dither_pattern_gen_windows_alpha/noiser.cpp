// noiser.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#undef UNICODE
#undef _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define AND 7
#define _CRT_RAND_S
#include <stdlib.h>
#include<Windows.h>
#include <iostream>
#include <atlimage.h>

#include <Gdiplusimaging.h>
unsigned char pattern[64];/* = {
	111,49,142,162,113,195,71,177,201,50,151,94,66,37,85,252,25,99,239,222,
32,250,148,19,38,106,220,170,194,138,13,167,125,178,79,15,65,173,123,87,
213,131,247,23,116,54,229,212,41,202,152,132,189,104,53,236,161,62,1,181,
77,241,147,68,2,244,56,91,230,5,204,28,187,101,144,206,33,92,190,107,223,
};/*
164,114,36,214,156,139,70,245,84,226,48,126,158,17,135,83,196,21,254,76,
45,179,115,12,40,169,105,253,176,211,59,100,180,145,122,172,97,235,129,
215,149,199,8,72,26,238,44,232,31,69,11,205,58,18,193,88,60,112,221,140,
86,120,153,208,130,243,160,224,110,34,248,165,24,234,184,52,198,171,6,108,
188,51,89,137,186,154,78,47,134,98,157,35,249,95,63,16,75,219,39,0,67,228,
121,197,240,3,74,127,20,227,143,246,175,119,200,251,103,146,14,209,174,
109,218,192,82,203,163,29,93,150,22,166,182,55,30,90,64,42,141,168,57,117,
46,216,233,61,128,81,237,217,118,159,255,185,27,242,102,4,133,73,191,9,
210,43,96,7,136,231,80,10,124,225,207,155,183,

};*/
unsigned char pattern_candidate[sizeof(pattern)];
unsigned char best_pattern_candidate[sizeof(pattern)];

unsigned int rnd()
{
	unsigned int o;
	rand_s(&o);
	return o;
}


double eval()
{
	double score= 0;

	int x, y;
	for(y=0;y<(AND+1);y++)
		for(x=0;x<(AND+1);x++)
		{
			int a = pattern_candidate[x+(AND+1)*y];
			int xx, yy;

			for(xx=-8;xx<9;xx++)
				for (yy = -8; yy < 9; yy++)
				{
					int b = pattern_candidate[((x+xx)&AND)+(AND+1)*((yy+y)&AND)];
					double aa = abs(a - b);// *(a - b);
					double dist =(xx*xx + yy * yy);
					if(dist<8*8)
					if(dist>0)score += aa/(dist);
				}
		}
	return score;
}

CImage *img;
void
display_img(unsigned char *img, int xr, int yr)
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

	StretchDIBits(dc, 512, 0, xr*2, yr*2, 0, 0, xr, yr, img, &info, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(GetConsoleWindow(), dc);

}

unsigned char data[1024 * 1024*3];

int GetGrayscale(COLORREF cr)
{
	BYTE byColor;

	byColor =
		(GetRValue(cr) * 0.30) +
		(GetGValue(cr) * 0.59) +
		(GetBValue(cr) * 0.11); 
		return byColor/4;

}
unsigned char im[1024 * 1024];
void conv()
{
	int xres = img->GetWidth();
	int yres = img->GetHeight();

	int y, x;
	for (y = 0; y < yres; y++)
		for (x = 0; x < xres; x++)
		{
			COLORREF c = img->GetPixel(x, y);
			int gray = GetGrayscale(c);
			im[x + y * xres] = gray;
		}
}

void dither()
{
	int xres = img->GetWidth();
	int yres = img->GetHeight();

	int y, x;
	for(y=0;y<yres;y++)
		for (x = 0; x < xres; x++)
		{
		//	COLORREF c = img->GetPixel(x, y);
			int gray = im[x + y * xres];// GetGrayscale(c);
			int p = x + y * xres;
			int px = x & AND;
			int py = y & AND;
			int o = 0;
			if (gray > pattern[px + (AND+1) * py])o = 255;
			data[p * 3] = o;
			data[p * 3+1] = o;
			data[p * 3+2] = o;
		}
	display_img(data, xres, yres);
}

void dither_img()
{
	int xres = img->GetWidth();
	int yres = img->GetHeight();

	int y, x;
	for (y = 0; y < yres; y++)
		for (x = 0; x < xres; x++)
		{
			//	COLORREF c = img->GetPixel(x, y);
			int gray = im[x + y * xres];// GetGrayscale(c);
			int p = x + y * xres;
			int px = x & AND;
			int py = y & AND;
			int o = 0;
			if (gray > pattern[px + (AND + 1) * py])o = 255;
			data[p * 3] = o;
			data[p * 3 + 1] = o;
			data[p * 3 + 2] = o;
			COLORREF c;
			c = o + (o << 8) + (o << 16);
			img->SetPixel(x, y, c);
		}
	display_img(data, xres, yres);
}

DEFINE_GUID(ImageFormatPNG, 0xb96b3caf, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e);

int main()
{
	img = new CImage;
	img->Load("c:/dev/lenna_o.png");
	conv();
	int i;
	if (1)	for (i = 0; i < sizeof(pattern); i++)
	{
		pattern[i] = i;// ^ (rnd() & 3);// >>= 2;
		/*
		pattern[i] = rnd()&63;
		if (i & 1)pattern[i] = i;
		*/
	}

	

if(1)	for (i = 0; i < sizeof(pattern); i++)
	{



		int j = rnd() & (sizeof(pattern)-1);
		int a = pattern[i];
		int b = pattern[j];
		pattern[j] = a;
		pattern[i] = b;
	}
dither();
//getchar();

	memcpy(pattern_candidate, pattern, sizeof(pattern));
	memcpy(best_pattern_candidate, pattern, sizeof(pattern));

	double best_score = eval();
	double real_best = best_score;
	int counter = 0;
	for (;;)
	{
		if((rnd()&1))
			memcpy(pattern_candidate, pattern, sizeof(pattern));
		if (!(rnd() & 255))
			memcpy(pattern_candidate, best_pattern_candidate, sizeof(pattern));

		
	//	a = counter & 63;
	//	b = counter >> 6;
	//	b &= 63;
		counter++;

		int a = rnd();
		int b = rnd();

		int op = rnd() % 3;

		if (op == 0)
		{
			b &= 63;
			a &= 63;


			int c = pattern_candidate[a];
			pattern_candidate[a] = pattern_candidate[b];
			pattern_candidate[b] = c;
		}

		if (op == 1)
		{
			b &= 7;
			a &= 7;

			for (int i = 0; i < 8; i++)
			{
				int c = pattern_candidate[a+8*i];
				pattern_candidate[a+8*i] = pattern_candidate[b+8*i];
				pattern_candidate[b+8*i] = c;
			}
		}

		if (op == 2)
		{
			b &= 7;
			a &= 7;

			for (int i = 0; i < 8; i++)
			{
				int c = pattern_candidate[a *8 + i];
				pattern_candidate[a * 8 + i] = pattern_candidate[b * 8 + i];
				pattern_candidate[b * 8 + i] = c;
			}
		}



		double score = eval();
		static int ttt=60000;

		if ((score > best_score)||((rnd()&0xffff)>ttt))
		{
		//	counter = 0;
			if((score > best_score))ttt++;
			int pr = 0;
			best_score = score;
			if (best_score > real_best)
			{
				pr = 1;
				real_best = best_score;
				memcpy(best_pattern_candidate, pattern_candidate, sizeof(pattern));
			}
			COORD coord;
			coord.X = 0;
			coord.Y = 0;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord );
			if (pr)dither();
		//	if(1)	printf("\rScore:%F %F, %d\r\n",score,real_best,ttt);
		if(0)	if (pr)
			{
				FILE *f = fopen("/dev/best_score", "rb");
				double bs = 1;
				if(f)fread(&bs, sizeof(bs), 1, f);
				if(f)fclose(f);
			//	if (!f)abort();
				if (bs < best_score)
				{
					printf("\r\n\r\nbest! %F %F\r\n",bs,best_score);
					f = fopen("/dev/best_score", "wb");
					fwrite(&best_score, sizeof(best_score), 1, f);
					fclose(f);
					char tmp[30];
					sprintf(tmp, "pat_%F.h",bs, best_score);
					f = fopen(tmp, "wb");

					for (i = 0; i < sizeof(pattern); i++)
					{
						if (!(i & AND))printf("        \r\n");
						fprintf(f,"%0d,", pattern_candidate[i]);
					}
					fclose(f);
				}
			}
			memcpy(pattern, pattern_candidate, sizeof(pattern));
		}
		static int iteration;
		iteration++;
		if (iteration == 1000000)
		{
			int id = rnd();
			char tmp[30];
			sprintf(tmp, "pat_%d.h", id);
			FILE* f = fopen(tmp, "wb");

			for (i = 0; i < sizeof(pattern); i++)
			{
				if (!(i & AND))printf("        \r\n");
				fprintf(f, "%0d,", pattern_candidate[i]);
			}
			fclose(f);
			dither_img();
			sprintf(tmp, "pat_%d.png", id);

			img->Save(tmp, ImageFormatPNG);


			exit(0);
		}
		if((iteration%50000)==0)
		printf("%d\r", iteration);
	}
}

// Programm ausführen: STRG+F5 oder "Debuggen" > Menü "Ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
