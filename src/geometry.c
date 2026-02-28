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

static Matrix *CreateIdentityMatrix(int16_t side_length)
{
	Matrix *m = malloc(sizeof(Matrix));

	m->rows = side_length;
	m->cols = side_length;

	m->data = malloc(side_length * side_length * sizeof(*m->data));

	for (int row = 0; row < m->rows; row++) {
		for (int col = 0; col < m->cols; col++) {
			if (row == col)
				MatrixSet(m, row, col, 1);
			else
				MatrixSet(m, row, col, 0);
		}
	}

	return m;
}

void MatrixFree(Matrix *m)
{
	free(m->data);
	free(m);
}

void MatrixPrint(Matrix m)
{
	printf("Matrix (%d, %d)\n", m.cols, m.rows);

	for (int row = 0; row < m.rows; row++) {
		for (int col = 0; col < m.cols; col++) {
			printf("%lf ", MatrixGet(m, row, col));
		}
		printf("\n");
	}
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

static void MatrixMultiplyScalar(Matrix *m, double mult)
{
	for (int row = 0; row < m->rows; row++) {
		for (int col = 0; col < m->cols; col++) {
			MatrixSet(m, row, col, MatrixGet(*m, row, col) * mult);
		}
	}
}

Matrix *MatrixTranspose(Matrix m)
{
	Matrix *trans = malloc(sizeof(Matrix));
	trans->rows = m.cols;
	trans->cols = m.rows;
	trans->data = malloc(trans->rows * trans->cols * sizeof(*trans->data));

	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			MatrixSet(trans, j, i, MatrixGet(m, i, j));
		}
	}

	return trans;
}

/* Augments two square matrices of the same size, used in MatrixInvert */
static Matrix *MatrixAugment(Matrix a, Matrix b)
{
	if (a.rows != a.cols || a.rows != b.rows || a.cols != b.cols)
		return NULL;

	Matrix *aug = malloc(sizeof(Matrix));
	aug->rows = a.rows;
	aug->cols = a.cols * 2;
	aug->data = malloc(aug->rows * aug->cols * sizeof(*aug->data));

	for (int row = 0; row < aug->rows; row++) {
		for (int col = 0; col < aug->cols; col++) {
			int i = col + aug->cols * row;

			if (col < aug->cols / 2)
				MatrixSet(aug, row, col, MatrixGet(a, row, col));
			else
				MatrixSet(aug, row, col, MatrixGet(b, row, col % (aug->cols / 2)));
		}
	}

	return aug;
}

/* For a matrix to be invertible it must be square, and the determinant must be nonzero */
void MatrixInvert(Matrix m)
{
	/* not a square matrix */
	if (m.rows != m.cols)
		return;

	Matrix *identity = CreateIdentityMatrix(m.rows);
	Matrix *aug = MatrixAugment(m, *identity);

	MatrixPrint(*aug);
}

