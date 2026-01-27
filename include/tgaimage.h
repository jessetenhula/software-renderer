#ifndef TGAIMAGE_H
#define TGAIMAGE_H

#include <stdint.h>
#include <stdbool.h>

/* Pixel storing channels in BRGA order */
typedef struct {
	uint8_t b, g, r, a;
} Pixel;

/* simple container for 32 bpp image */
typedef struct {
	uint16_t width;
	uint16_t height;
	Pixel *data;
} Image;

/* 32 bpp BGRA TGA Image loading/writing
 * restriction: loads all images assuming origin is at the bottom left */
void WriteTGAImage(const char *filename, Image img, bool rle);
Image *LoadTGAImage(const char *filename);

/* destructor for loaded image */
void ImageFree(Image *img);

/* Image data manipulation */
void SetPixel(Image img, int32_t x, int32_t y, Pixel color);
void DrawLine(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, Pixel color);

void DrawTriangle(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy, Pixel color);
void DrawTriangleBB(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy, Pixel color);

#endif /* TGAIMAGE_H */
