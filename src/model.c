#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "model.h"
#include "colors.h"

#define MODEL_DISTANCE 2

static Vertex RotateXZ(Vertex v, float rad)
{
	float c = cos(rad);
	float s = sin(rad);

	v.x = (v.x * c) - (v.z * s);
	v.z = (v.x * s) + (v.z * c);

	return v;
}

/* returns a 2d projected point in perspective */
static Point Project(Vertex v)
{
	Point p;

	/* render the model from a certain distance away */
	v.z += MODEL_DISTANCE;

	if (v.z == 0) {
		printf("depth zero\n");
		p.x = 0;
		p.y = 0;
		return p;
	}

	p.x = v.x / v.z;
	p.y = v.y / v.z;

	return p;
}

/* returns a 2d orthogonally projected point */
static Point ProjectOrthogonal(Vertex v)
{
	Point p;

	p.x = v.x;
	p.y = v.y;

	return p;
}

/* returns a point with the screen coordinates of a 2d projected point */
static Point Screen(Point p, uint32_t width, uint32_t height)
{
	p.x = ((p.x + 1) / 2) * (float) width;
	p.y = ((p.y + 1) / 2) * (float) height;

	return p;
}

void RenderWireframeTGA(Mesh *mesh, Image img)
{
	for (int32_t i = 0; i < mesh->tri_count; i++) {
		Triangle t = mesh->tris[i];

		Vertex verts[3] = {
			mesh->verts[t.v0],
			mesh->verts[t.v1],
			mesh->verts[t.v2]
		};

		for (int32_t j = 0; j < 3; j++) {
			Vertex startv = verts[j];
			Vertex endv = verts[(j + 1) % 3];

			Point startp = Screen(ProjectOrthogonal(startv), img.width, img.height);
			Point endp = Screen(ProjectOrthogonal(endv), img.width, img.height);

			DrawLine(img, startp.x, startp.y, endp.x, endp.y, RED);
		}
	}

	for (int32_t i = 0; i < mesh->vert_count; i++) {
		Vertex v = mesh->verts[i];
		Point p = Screen(ProjectOrthogonal(v), img.width, img.height);

		SetPixel(img, p.x, p.y, WHITE);
	}
}

