#ifndef TGAIMAGE_H
#define TGAIMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t id_length;			/* lengths of id field in bytes, 0 in our case */
	uint8_t color_map_type; 	/* 0, because we don't use color map */
	uint8_t image_type;			/* 2 for uncompressed true-color, 10 for rle true-color */ 
	uint8_t color_map_spec[5];	/* we don't use color map */

	/* image spec */
	uint16_t x_origin;			/* lower left coordinate */
	uint16_t y_origin;			/* lower left coordinate */
	uint16_t width;
	uint16_t height;
	uint8_t bpp;				/* bits per pixel, 32 for rgba with 8 bit channels */
	uint8_t image_descriptor;	/* specifies alpha channel depth, pixel ordering etc. */
} TGAHeader;

/* TGA saves pixels in BRGA order */
typedef struct {
	uint8_t b, g, r, a;
} Pixel;

/* simple container for 32 bpp image */
typedef struct {
	uint16_t width;
	uint16_t height;
	Pixel *data;
} Image;

/* rle packet header high-order bit */
typedef enum {
	RAW = 0,
	RLE = 1,
} PacketType;

/* 32 bpp BGRA TGA Image loading/writing */
void WriteTGAImage(const char *filename, Image img, bool rle);
Image *LoadTGAImage(const char *filename);

/* destructor for loaded image */
void ImageFree(Image *img);

/* Image data manipulation */
void SetPixel(Image img, int32_t x, int32_t y, Pixel color);
void DrawLine(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, Pixel color);

#endif /* TGAIMAGE_H */
