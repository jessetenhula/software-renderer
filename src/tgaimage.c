#include "tgaimage.h"

#define UNCOMPRESSED_TRUE_COLOR 2
#define RLE_TRUE_COLOR 10

#define RGBA_BITS_PER_PIXEL 32
#define ALPHA_BITS 8

/* helper macros */
#define swap_int32(a, b) { int32_t t = a; a = b; b = t; }

/* create basic TGA header for image with 32 bits per pixel data, uncompressed or rle */
TGAHeader CreateTGAHeader(uint16_t width, uint16_t height, bool rle)
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

/* helper functions for run-length-encoded images */
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

static bool PixelsEqual(Pixel a, Pixel b)
{
	if (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a)
		return true;
	else
		return false;
}

void WriteRLEData(FILE *file, TGAHeader h, Pixel *data)
{
	uint32_t data_size = h.width * h.height;
	uint32_t run_length = 0;
	uint8_t packet_h;

	for (int32_t start = 0; start < data_size; start += run_length) {
		Pixel px_start = data[start];
		run_length = 1;

		for (int32_t end = start + 1; end < data_size && run_length < 128; end++) {
			Pixel px_end = data[end];
			if (!PixelsEqual(px_start, px_end) || end % h.width == 0)
				break;

			run_length++;
		}

		if (run_length > 1) {
			/* rle */

			packet_h = EncodePacketHeader(RLE, run_length);
			fwrite(&packet_h, sizeof(packet_h), 1, file);
			fwrite(&data[start], sizeof(Pixel), 1, file);
		} else {
			/* raw */

			for (int32_t peek = start + 2; peek < data_size && run_length < 128; peek++) {
				Pixel px_end = data[peek - 1];
				Pixel px_peek = data[peek];
				if (PixelsEqual(px_end, px_peek) || peek % h.width == 0)
					break;

				run_length++;
			}

			packet_h = EncodePacketHeader(RAW, run_length);
			fwrite(&packet_h, sizeof(packet_h), 1, file);
			fwrite(&data[start], sizeof(Pixel), run_length, file);
		}
	}
}

void WriteTGAImage(const char *filename, TGAHeader h, Pixel *data)
{
	FILE *file = fopen(filename, "wb");

	if (h.bpp != RGBA_BITS_PER_PIXEL) {
		printf("WriteTGAImage: Only 32 bits per pixel supported\n");
		fclose(file);
		return;
	}

	WriteTGAHeader(file, h);

	if (h.image_type == UNCOMPRESSED_TRUE_COLOR) {
		fwrite(data, sizeof(Pixel), h.width * h.height, file);
	}

	if (h.image_type == RLE_TRUE_COLOR) {
		WriteRLEData(file, h, data);
	}

	fclose(file);
}


static Pixel *ReadRLEData(FILE *file, TGAHeader h)
{
	uint8_t packet_h;
	PacketType type;
	uint8_t count;

	Pixel *data = malloc(h.width * h.height * sizeof(Pixel));
	uint32_t data_index = 0;

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

Pixel *LoadTGAImage(const char *filename, TGAHeader *h)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		printf("LoadTGAImage: Unable to load file %s\n", filename);
		goto error;
	}

	fread(&h->id_length, sizeof(h->id_length), 1, file);
	fread(&h->color_map_type, sizeof(h->color_map_type), 1, file);
	fread(&h->image_type, sizeof(h->image_type), 1, file);

	if (h->image_type != UNCOMPRESSED_TRUE_COLOR && h->image_type != RLE_TRUE_COLOR) {
		printf("LoadTGAImage: Only run-length-encoded and uncompressed truecolor supported\n");
		goto error;
	}

	fread(&h->color_map_spec, sizeof(h->color_map_spec), 1, file);
	fread(&h->x_origin, sizeof(h->x_origin), 1, file);
	fread(&h->y_origin, sizeof(h->y_origin), 1, file);
	fread(&h->width, sizeof(h->width), 1, file);
	fread(&h->height, sizeof(h->height), 1, file);

	fread(&h->bpp, sizeof(h->bpp), 1, file);

	if (h->bpp != RGBA_BITS_PER_PIXEL) {
		printf("LoadTGAImage: Only 32 bits per pixel supported\n");
		goto error;
	}

	fread(&h->image_descriptor, sizeof(h->image_descriptor), 1, file);

	Pixel *data;
	if (h->image_type == RLE_TRUE_COLOR) {
		data = ReadRLEData(file, *h);
	} else if (h->image_type == UNCOMPRESSED_TRUE_COLOR) {
		data = malloc(h->width * h->height * sizeof(Pixel));
		fread(data, sizeof(Pixel), h->width * h->height, file);
	} else {
		printf("LoadTGAImage: Unsupported image type\n");
		goto error;
	}

	fclose(file);
	return data;
error:
	if (file != NULL)
		fclose(file);
	return NULL;
}

/* set pixel if in range of image data */
void SetPixel(Pixel *data, TGAHeader h, int32_t x, int32_t y, Pixel p)
{
	uint32_t i = h.width * y + x;

	if (i < 0 || i > h.width * h.height)
		return;

	data[i] = p;
}

/* draw line */
void DrawLine(Pixel *data, TGAHeader h, int32_t ax, int32_t ay, int32_t bx, int32_t by, Pixel color)
{
	/* line is actually a point */
	if (ax == bx && ay == by)
		SetPixel(data, h, ax, ay, color);

	bool steep = (abs(by - ay) > abs(bx - ax));
	if (steep) {
		swap_int32(ax, ay);
		swap_int32(bx, by);
	}

	if (ax > bx) {
		swap_int32(ax, bx);
		swap_int32(ay, by);
	}

	float dy = (by - ay) / (float) (bx - ax);

	float y = ay + 0.5;
	for (float x = ax; x <= bx; x += 1) {
		float t = (x - ax) / (float) (bx - ax);

		if (steep)
			SetPixel(data, h, y, x, color);
		else
			SetPixel(data, h, x, y, color);

		y += dy;
	}
}
