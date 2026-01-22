#include "tgaimage.h"

#define UNCOMPRESSED_TRUE_COLOR 2
#define RLE_TRUE_COLOR 10

#define RGBA_BITS_PER_PIXEL 32
#define ALPHA_BITS 8

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

			for (int32_t next = start + 2; next < data_size && run_length < 128; next++) {
				Pixel px_current = data[next - 1];
				Pixel px_next = data[next];

				if (PixelsEqual(px_current, px_next) || next % h.width == 0)
					break;

				run_length++;
			}

			packet_h = EncodePacketHeader(RAW, run_length);
			fwrite(&packet_h, sizeof(packet_h), 1, file);
			fwrite(&data[start], sizeof(Pixel), run_length, file);
		}
	}
}

void WriteTGAImageToFile(FILE *file, TGAHeader h, Pixel *data)
{
	if (h.bpp != RGBA_BITS_PER_PIXEL) {
		printf("WriteTGAImageToFile: Only 32 bits per pixel supported\n");
		return;
	}

	WriteTGAHeader(file, h);

	if (h.image_type == UNCOMPRESSED_TRUE_COLOR) {
		fwrite(data, sizeof(Pixel), h.width * h.height, file);
	}

	if (h.image_type == RLE_TRUE_COLOR) {
		WriteRLEData(file, h, data);
	}
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

Pixel *ReadTGAImageFromFile(FILE *file, TGAHeader *h)
{
	fread(&h->id_length, sizeof(h->id_length), 1, file);
	fread(&h->color_map_type, sizeof(h->color_map_type), 1, file);
	fread(&h->image_type, sizeof(h->image_type), 1, file);

	if (h->image_type != UNCOMPRESSED_TRUE_COLOR && h->image_type != RLE_TRUE_COLOR) {
		printf("ReadTGAImageFromFile: Only run-length-encoded and uncompressed truecolor supported\n");
		return NULL;
	}

	fread(&h->color_map_spec, sizeof(h->color_map_spec), 1, file);
	fread(&h->x_origin, sizeof(h->x_origin), 1, file);
	fread(&h->y_origin, sizeof(h->y_origin), 1, file);
	fread(&h->width, sizeof(h->width), 1, file);
	fread(&h->height, sizeof(h->height), 1, file);

	fread(&h->bpp, sizeof(h->bpp), 1, file);

	if (h->bpp != RGBA_BITS_PER_PIXEL) {
		printf("ReadTGAImageFromFile: Only 32 bits per pixel supported\n");
		return NULL;
	}

	fread(&h->image_descriptor, sizeof(h->image_descriptor), 1, file);

	if (h->image_type == RLE_TRUE_COLOR) {
		Pixel *data = ReadRLEData(file, *h);
		return data;
	} else if (h->image_type == UNCOMPRESSED_TRUE_COLOR) {
		Pixel *data = malloc(h->width * h->height * sizeof(Pixel));
		fread(data, sizeof(Pixel), h->width * h->height, file);
		return data;
	} else {
		printf("ReadTGAImageFromFile: Unsupported image type\n");
		return NULL;
	}
}
