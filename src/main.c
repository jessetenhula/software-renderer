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

	Matrix a;
	a.cols = 2;
	a.rows = 2;
	a.data = malloc(2 * 2 * sizeof(double));

	Matrix b;
	b.cols = 3;
	b.rows = 2;
	b.data = malloc(2 * 3 * sizeof(double));

	MatrixSet(&a, 0, 0, 1.0);
	MatrixSet(&a, 1, 0, 3.0);
	MatrixSet(&a, 0, 1, 2.0);
	MatrixSet(&a, 1, 1, 4.0);

	MatrixSet(&b, 0, 0, 0.0);
	MatrixSet(&b, 0, 1, 5.0);
	MatrixSet(&b, 0, 2, 2.0);
	MatrixSet(&b, 1, 0, 3.0);
	MatrixSet(&b, 1, 1, 1.0);
	MatrixSet(&b, 1, 2, 0.0);

	for (int i = 0; i < b.rows; i++) {
		for (int j = 0; j < b.cols; j++) {
			printf("%lf\n", MatrixGet(b, i, j));
		}
	}

	printf("\n");

	Matrix *c = MatrixMultiply(a, b);

	for (int i = 0; i < c->rows; i++) {
		for (int j = 0; j < c->cols; j++) {
			printf("%lf\n", MatrixGet(*c, i, j));
		}
	}

	return 0;
}
