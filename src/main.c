#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "tgaimage.h"
#include "objfile.h"
#include "model.h"
#include "colors.h"

#define WIDTH 800
#define HEIGHT 800

int main(int argc, char *argv[])
{
	srand(time(NULL));

	Image img;
	img.width = WIDTH;
	img.height = HEIGHT;

	img.data = malloc(img.width * img.height * sizeof(Pixel));
	for (int i = 0; i < img.width * img.height; i++)
		img.data[i] = BLACK;

	Mesh *mesh = LoadOBJFile("obj/diablo3_pose.obj"); 
	Render(mesh, img);
	MeshFree(mesh);

	WriteTGAImage("image/render.tga", img, true);

	return 0;
}
