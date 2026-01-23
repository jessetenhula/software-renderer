#include <stdio.h>
#include <stdint.h>

#include "tgaimage.h"

int main(int argc, char *argv[])
{
	FILE *in = fopen("image/floor_diffuse.tga", "rb");
	FILE *out = fopen("image/floor_diffuse_out.tga", "wb");

	if (in == NULL) {
		printf("Couldn't open input file\n");
		return -1;
	}

	TGAHeader header;
	Pixel *data = ReadTGAImageFromFile(in, &header);

	WriteTGAImageToFile(out, header, data); 

	fclose(in);
	fclose(out);

	return 0;
}
