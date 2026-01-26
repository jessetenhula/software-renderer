#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "model.h"
#include "colors.h"

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
static Point ProjectScreen(Vertex v, uint32_t width, uint32_t height)
{
	Point p;

	p.x = ((v.x + 1) / 2) * (float) width;
	p.y = ((v.y + 1) / 2) * (float) height;

	return p;
}

void RenderWireframe(Mesh *mesh, Image img)
{
	for (int32_t i = 0; i < mesh->tri_count; i++) {
		Triangle t = mesh->tris[i];

		for (int32_t j = 0; j < 3; j++) {
			Vertex startv = GetVertex(mesh, t, j);
			Vertex endv = GetVertex(mesh, t, (j + 1) % 3);

			Point startp = ProjectScreen(startv, img.width, img.height);
			Point endp = ProjectScreen(endv, img.width, img.height);

			DrawLine(img, startp.x, startp.y, endp.x, endp.y, RED);
		}
	}

	for (int32_t i = 0; i < mesh->vert_count; i++) {
		Vertex v = mesh->verts[i];
		Point p = ProjectScreen(v, img.width, img.height);

		SetPixel(img, p.x, p.y, WHITE);
	}
}

