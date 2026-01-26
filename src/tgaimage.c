#include "tgaimage.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define UNCOMPRESSED_TRUE_COLOR 2
#define RLE_TRUE_COLOR 10
#define RGBA_BITS_PER_PIXEL 32
#define ALPHA_BITS 8
#define swap_int32(a, b) { int32_t t = a; a = b; b = t; }

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

/* rle packet header high-order bit */
typedef enum {
	RAW = 0,
	RLE = 1,
} PacketType;

static void DecodePacketHeader(uint8_t packet_h, PacketType *type, uint8_t *count)
{
	/* packet type from high-order bit */
	if (packet_h & 128)
		*type = RLE;
	else
		*type = RAW;

	/* repetition count from 7 lower bits. */
	*count = (packet_h & 127) + 1;
}

static uint8_t EncodePacketHeader(PacketType type, uint8_t count)
{
	uint8_t packet_h = 0;

	if (type == RLE)
		packet_h |= 128;

	packet_h |= (count - 1) & 127;

	return packet_h;
}

static bool PixelsEqual(Pixel a, Pixel b)
{
	if (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a)
		return true;
	else
		return false;
}

static void WriteRLEData(FILE *file, Image img)
{
	uint32_t data_size = img.width * img.height;
	uint32_t run_length = 0;

	for (int32_t start = 0; start < data_size; start += run_length) {
		Pixel px_start = img.data[start];
		run_length = 1;

		for (int32_t end = start + 1; end < data_size && run_length < 128; end++) {
			Pixel px_end = img.data[end];
			if (!PixelsEqual(px_start, px_end) || end % img.width == 0)
				break;

			run_length++;
		}

		if (run_length > 1) {
			/* rle */

			uint8_t packet_h = EncodePacketHeader(RLE, run_length);
			fwrite(&packet_h, sizeof(packet_h), 1, file);
			fwrite(&img.data[start], sizeof(Pixel), 1, file);
		} else {
			/* raw */

			for (int32_t peek = start + 2; peek < data_size && run_length < 128; peek++) {
				Pixel px_end = img.data[peek - 1];
				Pixel px_peek = img.data[peek];
				if (PixelsEqual(px_end, px_peek) || peek % img.width == 0)
					break;

				run_length++;
			}

			uint8_t packet_h = EncodePacketHeader(RAW, run_length);
			fwrite(&packet_h, sizeof(packet_h), 1, file);
			fwrite(&img.data[start], sizeof(Pixel), run_length, file);
		}
	}
}

static TGAHeader CreateTGAHeader(uint16_t width, uint16_t height, bool rle)
{
	TGAHeader h = { 0 };

	if (rle)
		h.image_type = RLE_TRUE_COLOR;
	else
		h.image_type = UNCOMPRESSED_TRUE_COLOR;

	h.width = width;
	h.height = height;

	h.bpp = RGBA_BITS_PER_PIXEL;
	h.image_descriptor = ALPHA_BITS;

	return h;
}

static void WriteTGAHeader(FILE *file, TGAHeader h)
{
	fwrite(&h.id_length, sizeof(h.id_length), 1, file);
	fwrite(&h.color_map_type, sizeof(h.color_map_type), 1, file);
	fwrite(&h.image_type, sizeof(h.image_type), 1, file);
	fwrite(&h.color_map_spec, sizeof(h.color_map_spec), 1, file);

	fwrite(&h.x_origin, sizeof(h.x_origin), 1, file);
	fwrite(&h.y_origin, sizeof(h.y_origin), 1, file);
	fwrite(&h.width, sizeof(h.width), 1, file);
	fwrite(&h.height, sizeof(h.height), 1, file);

	fwrite(&h.bpp, sizeof(h.bpp), 1, file);
	fwrite(&h.image_descriptor, sizeof(uint8_t), 1, file);
}

/* write 32 bpp truecolor image, uncompressed or run-length-encoded */
void WriteTGAImage(const char *filename, Image img, bool rle)
{
	FILE *file = fopen(filename, "wb");

	TGAHeader h = CreateTGAHeader(img.width, img.height, rle);
	WriteTGAHeader(file, h);

	if (h.image_type == UNCOMPRESSED_TRUE_COLOR) {
		fwrite(img.data, sizeof(Pixel), img.width * img.height, file);
	} else if (h.image_type == RLE_TRUE_COLOR) {
		WriteRLEData(file, img);
	}

	fclose(file);
}


static Pixel *ReadRLEData(FILE *file, TGAHeader h)
{
	Pixel *data = malloc(h.width * h.height * sizeof(Pixel));
	uint32_t data_index = 0;

	uint8_t packet_h;
	PacketType type;
	uint8_t count;
	do {
		fread(&packet_h, sizeof(packet_h), 1, file);
		DecodePacketHeader(packet_h, &type, &count);

		if (type == RAW) {
			for (int32_t i = 0; i < count; i++)
				fread(&data[data_index++], sizeof(Pixel), 1, file);
		} else if (type == RLE) {
			Pixel color;
			fread(&color, sizeof(Pixel), 1, file);
			for (int i = 0; i < count; i++)
				data[data_index++] = color;
		}
	} while (data_index < h.width * h.height);

	return data;
}

/* load 32 bpp truecolor image, uncompressed or run-length-encoded */
Image *LoadTGAImage(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		printf("LoadTGAImage: Unable to load file %s\n", filename);
		goto error;
	}

	TGAHeader h = { 0 };

	fread(&h.id_length, sizeof(h.id_length), 1, file);
	fread(&h.color_map_type, sizeof(h.color_map_type), 1, file);
	fread(&h.image_type, sizeof(h.image_type), 1, file);

	if (h.image_type != UNCOMPRESSED_TRUE_COLOR && h.image_type != RLE_TRUE_COLOR) {
		printf("LoadTGAImage: Only run-length-encoded and uncompressed truecolor supported\n");
		goto error;
	}

	fread(&h.color_map_spec, sizeof(h.color_map_spec), 1, file);
	fread(&h.x_origin, sizeof(h.x_origin), 1, file);
	fread(&h.y_origin, sizeof(h.y_origin), 1, file);
	fread(&h.width, sizeof(h.width), 1, file);
	fread(&h.height, sizeof(h.height), 1, file);

	fread(&h.bpp, sizeof(h.bpp), 1, file);

	if (h.bpp != RGBA_BITS_PER_PIXEL) {
		printf("LoadTGAImage: Only 32 bits per pixel supported\n");
		goto error;
	}

	fread(&h.image_descriptor, sizeof(h.image_descriptor), 1, file);

	Pixel *data;
	if (h.image_type == RLE_TRUE_COLOR) {
		data = ReadRLEData(file, h);
	} else if (h.image_type == UNCOMPRESSED_TRUE_COLOR) {
		data = malloc(h.width * h.height * sizeof(Pixel));
		fread(data, sizeof(Pixel), h.width * h.height, file);
	} else {
		printf("LoadTGAImage: Unsupported image type\n");
		goto error;
	}

	Image *img = malloc(sizeof(Image));
	img->width = h.width;
	img->height = h.height;
	img->data = data;

	fclose(file);
	return img;
error:
	if (file != NULL)
		fclose(file);
	return NULL;
}

void ImageFree(Image *img)
{
	if (img->data != NULL)
		free(img->data);

	free(img);
}

/* drawing */

void SetPixel(Image img, int32_t x, int32_t y, Pixel color)
{
	uint32_t i = img.width * y + x;

	if (i < 0 || i > img.width * img.height)
		return;

	img.data[i] = color;
}

void DrawLine(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, Pixel color)
{
	/* line is actually a point */
	if (ax == bx && ay == by)
		SetPixel(img, ax, ay, color);

	bool steep = (abs(by - ay) > abs(bx - ax));
	if (steep) {
		swap_int32(ax, ay);
		swap_int32(bx, by);
	}

	if (ax > bx) {
		swap_int32(ax, bx);
		swap_int32(ay, by);
	}

	float y = ay; 
	float dy = (by - ay) / (float) (bx - ax);
	for (float x = ax; x <= bx; x += 1) {
		if (steep)
			SetPixel(img, y, x, color);
		else
			SetPixel(img, x, y, color);

		y += dy;
	}
}

/* gets the x at a given y on a line defined by a point and its slope */
static float GetLineX(int32_t ax, int32_t ay, float slope, int32_t y)
{
	return (slope * ax - ay + y) / slope;
}

static void DrawScanline(Image img, int32_t ax, int32_t bx, int32_t y, Pixel color)
{
	if (ax > bx)
		swap_int32(ax, bx);

	for (int32_t x = ax; x <= bx; x++) {
		SetPixel(img, x, y, color);
	}
}

void DrawTriangle(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy, Pixel color)
{
	/* sort vertices by y position */
	if (by > ay) {
		swap_int32(by, cy);
		swap_int32(bx, cx);
	}

	if (cy > ay) {
		swap_int32(ay, cy);
		swap_int32(ax, cx);
	}

	if (cy > by) {
		swap_int32(by, cy);
		swap_int32(bx, cx);
	}

	int32_t total_height = ay - cy;

	/* top half of triangle */
	if (ay != by) {
		int32_t segment_height = ay - by;
		for (int32_t y = ay; y >= by; y--) {
			int32_t x1 = ax + ((bx - ax) * (ay - y)) / segment_height;
			int32_t x2 = ax + ((cx - ax) * (ay - y)) / total_height;

			DrawScanline(img, x1, x2, y, color);
		}
	}

	/* bottom half of triangle */
	if (by != cy) {
		int32_t segment_height = by - cy;
		for (int32_t y = by - 1; y >= cy; y--) {
			int32_t x1 = bx + ((cx - bx) * (by - y)) / segment_height;
			int32_t x2 = ax + ((cx - ax) * (ay - y)) / total_height;

			DrawScanline(img, x1, x2, y, color);
		}
	}

}

