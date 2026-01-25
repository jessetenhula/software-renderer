#ifndef OBJFILE_H
#define OBJFILE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define TOKEN_MAX_LENGTH 100
#define END_OF_LINE -1

#define INITIAL_VERTEX_CAPACITY 128
#define INITIAL_FACE_CAPACITY 128
#define FACE_MAX_VERTICES 4

typedef struct {
	float x, y, z, w;
} Vertex;

/* Face defined by vertex indices */
typedef struct {
	uint32_t count;
	int32_t *v_is;
} Face;

typedef struct {
	uint32_t v_count;
	uint32_t v_capacity;
	Vertex *vs;

	uint32_t f_count;
	uint32_t f_capacity;
	Face *fs;
} OBJ;

/* loads OBJ file. rejects files that have faces defined with negative vertex indices */
OBJ *LoadOBJFile(const char *filename);

/* destructor for loaded OBJ file */
void OBJFree(OBJ *obj);

#endif /* OBJFILE_H */
