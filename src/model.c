#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "model.h"
#include "colors.h"
#include "geometry.h"

/* since images are loaded as TGA and TGA images have their origin at the bottom left,
 * Projection does not invert the y axis*/

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

void RenderWireframe(Mesh *mesh, Image img)
{
	for (int32_t i = 0; i < mesh->face_count; i++) {
		Face f = mesh->faces[i];

		for (int32_t j = 0; j < 3; j++) {
			Vertex av = GetVertex(mesh, f, j);
			Vertex bv = GetVertex(mesh, f, (j + 1) % 3);

			Vec3 a = Project(av, img.width, img.height);
			Vec3 b = Project(bv, img.width, img.height);

			DrawLine(img, a.x, a.y, b.x, b.y, RED);
		}
	}

	for (int32_t i = 0; i < mesh->vertex_count; i++) {
		Vertex v = mesh->vertices[i];
		Vec3 p = Project(v, img.width, img.height);

		SetPixel(img, p.x, p.y, WHITE);
	}
}

static Pixel RandColor()
{
	Pixel color = {
		.r = 128,
		.g = (rand() % 128) + 128,
		.b = (rand() % 128) + 128,
		.a = 255
	};

	return color;
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
