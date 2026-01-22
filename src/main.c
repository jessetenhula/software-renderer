#include <stdio.h>
#include <stdint.h>

#include "tgaimage.h"

//void print_tga_header(TGAHeader header)
//{
//	printf("w %d, h %d\n", header.width, header.height);
//	printf("x_origin %d, y_origin %d\n", header.x_origin, header.y_origin);
//	printf("image type %d\n", header.image_type);
//}

int main(int argc, char *argv[])
{
	FILE *in = fopen("floor_diffuse.tga", "rb");

	TGAHeader header;
	Pixel *data = ReadTGAImageFromFile(in, &header);

	FILE *out = fopen("floor_diffuse_out.tga", "wb");

	WriteTGAImageToFile(out, header, data); 

	fclose(in);
	fclose(out);

	return 0;
}
