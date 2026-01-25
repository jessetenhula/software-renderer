#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "tgaimage.h"
#include "objfile.h"

#define WIDTH 800
#define HEIGHT 800
#define MODEL_DISTANCE 1.5

Pixel black = { 0, 0, 0, 255 };
Pixel white = { 255, 255, 255, 255 };
Pixel red = { 0, 0, 255, 255 };
Pixel green = { 0, 255, 0, 255 };
Pixel blue = { 255, 0, 0, 255 };

typedef struct {
	float x, y;
} Point;

Vertex RotateXZ(Vertex v, float rad)
{
	v.x = v.x * (cos(rad) + sin(rad));
	v.z = v.z * (cos(rad) - sin(rad));

	return v;
}

/* returns a 2d projected point */
Point Project(Vertex v)
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

/* returns a point with the screen coordinates of a 2d projected point */
Point Screen(Point p, uint32_t width, uint32_t height)
{
	Point s;

	s.x = ((p.x + 1) / 2) * (float) width;
	s.y = ((p.y + 1) / 2) * (float) height;

	return s;
}

void RenderWireframeTGA(OBJ *obj, Pixel *canvas, TGAHeader h)
{
	int32_t width = h.width;
	int32_t height = h.height;

	for (int32_t i = 0; i < obj->f_count; i++) {
		Face f = obj->fs[i];
		for (int32_t j = 0; j < f.count; j++) {
			Vertex startv = obj->vs[f.v_is[j] - 1];
			Vertex endv = obj->vs[f.v_is[(j + 1) % f.count] - 1];

			float rot = 3.14 / 2;
			startv = RotateXZ(startv, rot);
			endv = RotateXZ(endv, rot);

			Point startp = Screen(Project(startv), width, height);
			Point endp = Screen(Project(endv), width, height);

			DrawLine(canvas, h, (int32_t) startp.x, (int32_t) startp.y, (int32_t) endp.x, (int32_t) endp.y, red);
		}
	}

//	for (int32_t i = 0; i < obj->v_count; i++) {
//		Vertex v = obj->vs[i];
//		Point screen_point = Screen(Project(v), width, height);
//		SetPixel(canvas, h, (int32_t) screen_point.x, (int32_t) screen_point.y, white);
//	}
}

int main(int argc, char *argv[])
{

	/* file setup */
	TGAHeader header = CreateTGAHeader(WIDTH, HEIGHT, 1);

	Pixel *img = malloc(WIDTH * HEIGHT * sizeof(Pixel));
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		img[i] = black;

	OBJ *obj = LoadOBJFile("obj/diablo3_pose.obj"); 

	RenderWireframeTGA(obj, img, header);

	WriteTGAImage("image/render.tga", header, img);

	return 0;
}
