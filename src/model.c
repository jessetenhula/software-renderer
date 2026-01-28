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

/* projects vertex to screen coordinates orthogonally */
static Vec2 ProjectScreen(Vertex v, uint32_t width, uint32_t height)
{
	Vec2 p;

	p.x = ((v.x + 1) / 2) * (float) width;
	p.y = ((v.y + 1) / 2) * (float) height;

	return p;
}

void RenderWireframe(Mesh *mesh, Image img)
{
	for (int32_t i = 0; i < mesh->face_count; i++) {
		Face f = mesh->faces[i];

		for (int32_t j = 0; j < 3; j++) {
			Vertex av = GetVertex(mesh, f, j);
			Vertex bv = GetVertex(mesh, f, (j + 1) % 3);

			Vec2 a = ProjectScreen(av, img.width, img.height);
			Vec2 b = ProjectScreen(bv, img.width, img.height);

			DrawLine(img, a.x, a.y, b.x, b.y, RED);
		}
	}

	for (int32_t i = 0; i < mesh->vertex_count; i++) {
		Vertex v = mesh->vertices[i];
		Vec2 p = ProjectScreen(v, img.width, img.height);

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
	for (int32_t i = 0; i < mesh->face_count; i++) {
		Face f = mesh->faces[i];

		Vec2 a = ProjectScreen(GetVertex(mesh, f, 0), img.width, img.height);
		Vec2 b = ProjectScreen(GetVertex(mesh, f, 1), img.width, img.height);
		Vec2 c = ProjectScreen(GetVertex(mesh, f, 2), img.width, img.height);

		DrawTriangle(img, a.x, a.y, b.x, b.y, c.x, c.y, RandColor());
	}

}
