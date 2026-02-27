#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "geometry.h"

/* Vec2 */

Vec2 Vec2Add(Vec2 a, Vec2 b)
{
	return (Vec2) { a.x + b.x, a.y + b.y };
}

Vec2 Vec2Sub(Vec2 a, Vec2 b)
{
	return (Vec2) { a.x - b.x, a.y - b.y };
}

double Vec2Dot(Vec2 a, Vec2 b)
{
	return (a.x * b.x) + (a.y + b.y);
}

/* Vec3 */

Vec3 Vec3Add(Vec3 a, Vec3 b)
{
	return (Vec3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 Vec3Sub(Vec3 a, Vec3 b)
{
	return (Vec3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

double Vec3Dot(Vec3 a, Vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 Vec3Cross(Vec3 a, Vec3 b)
{
	return (Vec3) {
		a.y * b.z - b.y * a.z,
		a.z * b.x - b.z * a.x,
		a.x * b.y - b.x * a.y
	};
}

/* Vec4 */

Vec4 Vec4Add(Vec4 a, Vec4 b)
{
	return (Vec4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vec4 Vec4Sub(Vec4 a, Vec4 b)
{
	return (Vec4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

double Vec4Dot(Vec4 a, Vec4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w + b.w);
}


/* Matrix */

double MatrixGet(Matrix m, uint16_t row, uint16_t col)
{
	if ((col + m.cols * row) >= m.cols * m.rows)
		printf("dangerous MatrixGet!\n");

	return m.data[col + m.cols * row];
}

void MatrixSet(Matrix *m, uint16_t row, uint16_t col, double val)
{
	if ((col + m->cols * row) >= m->cols * m->rows)
		printf("dangerous MatrixSet!\n");

	m->data[col + m->cols * row] = val;
}

Matrix *MatrixMultiply(Matrix a, Matrix b)
{
	/* For matrix multiplication, the number of columns in the first matrix must be equal to the number of rows in the second matrix.
	 * The result matrix has the number of rows of the first and the number of columns of the second matrix.* */
	if (a.cols != b.rows)
		return NULL;

	Matrix *c = malloc(sizeof(Matrix));
	c->rows = a.rows;
	c->cols = b.cols;
	c->data = malloc(c->rows * c->cols * sizeof(*(c->data)));

	int n = a.cols;

	for (int i = 0; i < c->rows; i++) {
		for (int j = 0; j < c->cols; j++) {
			int accumulator = 0;
			for (int k = 0; k < n; k++) {
				accumulator += MatrixGet(a, i, k) * MatrixGet(b, k, j);
			}
			MatrixSet(c, i, j, accumulator);
		}
	}

	return c;
}

void MatrixInvert(Matrix m)
{
}


void MatrixTranspose(Matrix m)
{
}
