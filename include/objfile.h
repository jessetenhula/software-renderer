#ifndef OBJFILE_H
#define OBJFILE_H

#include <stdint.h>

typedef struct {
	float x, y, z, w;
} Vertex;

typedef struct {
	uint32_t v0, v1, v2;
} Triangle;

typedef struct {
	uint32_t vert_count;
	Vertex *verts;

	uint32_t tri_count;
	Triangle *tris;
} Mesh;

/* loads OBJ file. rejects files that have faces defined with negative vertex indices */
Mesh *LoadOBJFile(const char *filename);

/* destructor for loaded mesh */
void MeshFree(Mesh *mesh);

#endif /* OBJFILE_H */
