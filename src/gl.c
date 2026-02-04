#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "gl.h"
#include "colors.h"
#include "geometry.h"

/* helpers */

#define swap_int32(a, b) { int32_t t = a; a = b; b = t; }

static int32_t min_int32(int32_t a, int32_t b)
{
	if (a < b)
		return a;
	else
		return b;
}

static int32_t max_int32(int32_t a, int32_t b)
{
	if (a > b)
		return a;
	else
		return b;
}

static Pixel RandColor()
{
	Pixel color = {
		.r = (rand() % 255),
		.g = (rand() % 255), 
		.b = (rand() % 255),
		.a = 255
	};

	return color;
}

/* drawing */

static void DrawLine(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, Pixel color)
{
	/* line is actually a point */
	if (ax == bx && ay == by)
		SetPixel(img, ax, ay, color);

	bool steep = (abs(by - ay) > abs(bx - ax));
	if (steep) {
		swap_int32(ax, ay);
		swap_int32(bx, by);
	}

	if (ax > bx) {
		swap_int32(ax, bx);
		swap_int32(ay, by);
	}

	float y = ay; 
	float dy = (by - ay) / (float) (bx - ax);
	for (float x = ax; x <= bx; x += 1) {
		if (steep)
			SetPixel(img, y, x, color);
		else
			SetPixel(img, x, y, color);

		y += dy;
	}
}

static void DrawScanline(Image img, int32_t ax, int32_t bx, int32_t y, Pixel color)
{
	if (ax > bx)
		swap_int32(ax, bx);

	for (int32_t x = ax; x <= bx; x++) {
		SetPixel(img, x, y, color);
	}
}

static void DrawTriangle(Image img, int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy, Pixel color)
{
	/* sort vertices by y position */
	if (by > ay) {
		swap_int32(ay, by);
		swap_int32(ax, bx);
	}

	if (cy > ay) {
		swap_int32(ay, cy);
		swap_int32(ax, cx);
	}

	if (cy > by) {
		swap_int32(cy, by);
		swap_int32(cx, bx);
	}

	int32_t total_height = ay - cy;

	/* top half of triangle */
	if (ay != by) {
		int32_t segment_height = ay - by;
		for (int32_t y = ay; y >= by; y--) {
			int32_t x1 = ax + ((bx - ax) * (ay - y)) / segment_height;
			int32_t x2 = ax + ((cx - ax) * (ay - y)) / total_height;

			DrawScanline(img, x1, x2, y, color);
		}
	}

	/* bottom half of triangle */
	if (by != cy) {
		int32_t segment_height = by - cy;
		for (int32_t y = by - 1; y >= cy; y--) {
			int32_t x1 = bx + ((cx - bx) * (by - y)) / segment_height;
			int32_t x2 = ax + ((cx - ax) * (ay - y)) / total_height;

			DrawScanline(img, x1, x2, y, color);
		}
	}
}

static double TriangleSignedArea(int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy)
{
	return 0.5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));;
}

static void DrawTriangleBoundingBox(Image img, uint8_t *z_buffer, int32_t ax, int32_t ay, uint8_t az, int32_t bx, int32_t by, uint8_t bz, int32_t cx, int32_t cy, uint8_t cz, Pixel color)
{
	int32_t min_x = min_int32(min_int32(ax, bx), cx);
	int32_t min_y = min_int32(min_int32(ay, by), cy);

	int32_t max_x = max_int32(max_int32(ax, bx), cx);
	int32_t max_y = max_int32(max_int32(ay, by), cy);

	double sarea = TriangleSignedArea(ax, ay, bx, by, cx, cy);

	for (int32_t px = min_x; px <= max_x; px++) {
		for (int32_t py = min_y; py <= max_y; py++) {
			double l1 = TriangleSignedArea(px, py, bx, by, cx, cy) / sarea;
			double l2 = TriangleSignedArea(px, py, cx, cy, ax, ay) / sarea;
			double l3 = TriangleSignedArea(px, py, ax, ay, bx, by) / sarea;

			if (l1 < -0.001 || l2 < -0.001 || l3 < -0.001)
				continue;

			double depth = l1*az + l2*bz + l3*cz;
			if (z_buffer[img.width * py + px] < depth)
				z_buffer[img.width * py + px] = depth;
			else
				continue;

			SetPixel(img, px, py, color);
		}
	}
}

/* render */
static Vertex RotateXZ(Vertex v, float rad)
{
	float c = cos(rad);
	float s = sin(rad);

	v.x = (v.x * c) - (v.z * s);
	v.z = (v.x * s) + (v.z * c);

	return v;
}

/* projects vertex to screen coordinates orthogonally, includes depth */
static Vec3 Project(Vertex v, uint32_t width, uint32_t height)
{
	Vec3 p;

	p.x = ((v.x + 1) / 2) * (float) width;
	p.y = ((v.y + 1) / 2) * (float) height;
	p.z = ((v.z + 1) / 2) * 255;

	return p;
}

void Render(Mesh *mesh, Image img)
{
	uint8_t *z_buffer = calloc(img.width * img.height, sizeof(double));

	for (int32_t i = 0; i < mesh->face_count; i++) {
		Face f = mesh->faces[i];

		Vertex av = GetVertex(mesh, f, 0);
		Vertex bv = GetVertex(mesh, f, 1);
		Vertex cv = GetVertex(mesh, f, 2);

		Vec3 a = Project(av, img.width, img.height);
		Vec3 b = Project(bv, img.width, img.height);
		Vec3 c = Project(cv, img.width, img.height);

		DrawTriangleBoundingBox(img, z_buffer, a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z, RandColor());
	}
}
