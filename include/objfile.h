#ifndef OBJFILE_H
#define OBJFILE_H

#include <stdint.h>

typedef struct {
	float x, y, z, w;
} Vertex;

typedef struct {
	uint32_t vertex_indices[3];
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

/* convenience function to get the vertex from a triangle */
Vertex GetVertex(Mesh *mesh, Triangle t, uint32_t i);

#endif /* OBJFILE_H */
