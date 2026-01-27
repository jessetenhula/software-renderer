#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "tgaimage.h"
#include "objfile.h"
#include "model.h"
#include "colors.h"

#define WIDTH 800
#define HEIGHT 800

int main(int argc, char *argv[])
{
	Image img;
	img.width = WIDTH;
	img.height = HEIGHT;

	img.data = malloc(img.width * img.height * sizeof(Pixel));
	for (int i = 0; i < img.width * img.height; i++)
		img.data[i] = BLACK;

//	Mesh *mesh = LoadOBJFile("obj/diablo3_pose.obj"); 
//	RenderWireframe(mesh, img);
//	MeshFree(mesh);

	DrawTriangle(img, 20, 20, 180, 400, 360, 50, WHITE);
	DrawTriangle(img, 200, 20, 240, 400, 360, 50, BLUE);
	DrawTriangle(img, 20, 600, 180, 601, 360, 600, RED);

	WriteTGAImage("image/render.tga", img, true);

	return 0;
}
