#ifndef MODEL_H
#define MODEL_H

#include "tgaimage.h"
#include "objfile.h"

typedef struct {
	float x, y;
} Point;

void RenderWireframeTGA(Mesh *mesh, Image img);

#endif /* MODEL_H */
