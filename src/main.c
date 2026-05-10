#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "tgaimage.h"
#include "model.h"
#include "gl.h"
#include "colors.h"

#define WIDTH 800
#define HEIGHT 800

#include <geometry.h>

int main(int argc, char *argv[])
{
	/*
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

	free(img.data);

	*/

	Matrix b;
	b.cols = 3;
	b.rows = 3;
	b.data = malloc(3 * 3 * sizeof(double));

	MatrixSet(&b, 0, 0, 5.0);
	MatrixSet(&b, 0, 1, 7.0);
	MatrixSet(&b, 0, 2, 9.0);

	MatrixSet(&b, 1, 0, 4.0);
	MatrixSet(&b, 1, 1, 3.0);
	MatrixSet(&b, 1, 2, 8.0);

	MatrixSet(&b, 2, 0, 7.0);
	MatrixSet(&b, 2, 1, 5.0);
	MatrixSet(&b, 2, 2, 6.0);

	MatrixPrint(b);
	Matrix *inv_b = MatrixInvert(b);
	MatrixPrint(*inv_b);

	return 0;
}
