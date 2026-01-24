#include <stdio.h>
#include <stdint.h>

#include "tgaimage.h"

#define WIDTH 64
#define HEIGHT 64

Pixel black = { 0, 0, 0, 255 };
Pixel white = { 255, 255, 255, 255 };
Pixel red = { 0, 0, 255, 255 };
Pixel green = { 0, 255, 0, 255 };
Pixel blue = { 255, 0, 0, 255 };

int main(int argc, char *argv[])
{
	FILE *out = fopen("image/lines.tga", "wb");

	TGAHeader header = CreateTGAHeader(WIDTH, HEIGHT, 1);

	Pixel *data = malloc(WIDTH * HEIGHT * sizeof(Pixel));
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		data[i] = black;

	DrawLine(data, header, 0, 0, 30, 14, blue);
	DrawLine(data, header, 4, 4, 8, 30, red);
	DrawLine(data, header, 4, 4, 8, 0, white);

	WriteTGAImageToFile(out, header, data); 

	fclose(out);

	return 0;
}
