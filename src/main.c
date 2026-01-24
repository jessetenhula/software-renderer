#include <stdio.h>
#include <stdint.h>

#include "tgaimage.h"

#define WIDTH 64
#define HEIGHT 64

Pixel white = { 255, 255, 255, 255 };
Pixel black = { 0, 0, 0, 255 };

//void DrawLine(Pixel *data, TGAHeader h, uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by)
//{
//	float inc = 0.01;
//
//	for (float t = 0; t <= 1; t += inc) {
//		uint32_t x = (1.0 - t) * ax + t * bx;
//		uint32_t y = (1.0 - t) * ay + t * by;
//
//		SetPixel(data, h, x, y, white);
//	}
//}

void DrawLine(Pixel *data, TGAHeader h, uint32_t ax, uint32_t ay, uint32_t bx, uint32_t by)
{
	if (ax > bx) {
		uint32_t t = ax;
		ax = bx;
		bx = t;

		t = ay;
		ay = by;
		by = t;
	}

	float dy = (by - ay) / (float) (bx - ax);

	for (int32_t x = ax; x <= bx; x++) {
		float y = x * dy;
		SetPixel(data, h, x, (uint32_t) (y + 0.5), white);
	}
}

int main(int argc, char *argv[])
{
	FILE *out = fopen("image/frame_buffer.tga", "wb");

	TGAHeader header = CreateTGAHeader(WIDTH, HEIGHT, 1);

	Pixel *data = malloc(WIDTH * HEIGHT * sizeof(Pixel));
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		data[i] = black;

	DrawLine(data, header, 55, 3, 0, 0);

	WriteTGAImageToFile(out, header, data); 

	fclose(out);

	return 0;
}
