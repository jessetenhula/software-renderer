#ifndef OBJFILE_H
#define OBJFILE_H

#include <stdint.h>

typedef struct {
	float x, y, z, w;
} Vertex;

/* triangle face */
typedef struct {
	uint32_t vertex_indices[3];
} Face;

typedef struct {
	uint32_t vertex_count;
	Vertex *vertices;

	uint32_t face_count;
	Face *faces;
} Mesh;

/* loads OBJ file. rejects files that have faces defined with negative vertex indices */
Mesh *LoadOBJFile(const char *filename);

/* destructor for loaded mesh */
void MeshFree(Mesh *mesh);

/* convenience function to get the vertex from a triangle */
Vertex GetVertex(Mesh *mesh, Face f, uint32_t i);

#endif /* OBJFILE_H */
