#include "objfile.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define TOKEN_MAX_LENGTH 100
#define END_OF_LINE -1
#define OBJ_ARRAY_DEFAULT_CAPACITY 128

/* intermediary struct for loading OBJ file */
typedef struct {
	uint32_t vert_count;
	uint32_t vert_capacity;
	Vertex *verts;

	uint32_t tri_count;
	uint32_t tri_capacity;
	Triangle *tris;
} OBJ;

static void ReadComment(FILE *file)
{
	int32_t c;
	bool continue_line = false;

	do {
		c = fgetc(file);

		if (c == '\\')
			continue_line == true;

		if (c == '\n') {
			if (!continue_line)
				break;
			else
				continue_line = false;
		}
	} while (c != EOF);
}

/* return END_OF_LINE (-1) if read token is at the end of the line or file, otherwise return 0 */
static int32_t ReadToken(FILE *file, unsigned char *buffer, uint32_t max_size)
{
	int32_t c;
	int32_t i = 0;

	do {
		c = fgetc(file);
	} while (c != EOF && isspace(c));

	if (c == EOF)
		return END_OF_LINE;

	do {
		buffer[i++] = c;
		c = fgetc(file);
	} while (c != EOF && !isspace(c) && i < max_size - 1);

	buffer[i] = '\0';

	if (c == EOF || c == '\n')
		return END_OF_LINE;

	return 0;
}

static void ReadVertex(FILE *file, OBJ *obj)
{
	Vertex v = { 0, 0, 0, 1 };

	unsigned char token[TOKEN_MAX_LENGTH];

	ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.x = atof(token);

	ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.y = atof(token);

	int32_t ret_code = ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.z = atof(token);

	/* optional w value */
	if (ret_code != END_OF_LINE) {
		ReadToken(file, token, TOKEN_MAX_LENGTH);
		v.w = atof(token);
	}

	if (obj->vert_count == obj->vert_capacity) {
		uint32_t new_capacity = obj->vert_capacity * 2;
		obj->verts = realloc(obj->verts, new_capacity * sizeof(Vertex));
		obj->vert_capacity = new_capacity;
	}
	obj->verts[obj->vert_count++] = v;
}

static void ReadFace(FILE *file, OBJ *obj)
{
	/* assume face is a triangle */
	int32_t *indices = malloc(3 * sizeof(int32_t));
	uint32_t capacity = 3;
	uint32_t count = 0;

	unsigned char token[TOKEN_MAX_LENGTH];
	int32_t ret_code = 0;
	do {
		if (count == capacity) 
			indices = realloc(indices, capacity * 2 * sizeof(int32_t));

		ret_code = ReadToken(file, token, TOKEN_MAX_LENGTH);
		int32_t i = atoi(token);

		/* faces can be defined with negative indices but I don't want to deal with that */
		if (i < 0) {
			free(indices);
			return;
		}

		indices[count++] = i - 1;
	} while (ret_code != END_OF_LINE);

	if (count < 3) {
		free(indices);
		return;
	}

	/* fan triangulation */
	for (int32_t i = 1; i < count - 1; i++) {
		Triangle t = { 
			indices[0],
			indices[i],
			indices[i + 1]
		};

		if (obj->tri_count == obj->tri_capacity) {
			uint32_t new_capacity = obj->tri_capacity * 2;
			obj->tris = realloc(obj->tris, new_capacity * sizeof(Triangle));
			obj->tri_capacity = new_capacity;
		}
		obj->tris[obj->tri_count++] = t;
	}

	free(indices);
}

static OBJ *InitializeOBJ(uint32_t capacity)
{
	OBJ *obj = malloc(sizeof(OBJ));

	obj->vert_count = 0;
	obj->vert_capacity = capacity;;
	obj->verts = malloc(capacity * sizeof(Vertex));

	obj->tri_count = 0;
	obj->tri_capacity = capacity;
	obj->tris = malloc(capacity * sizeof(Triangle));

	return obj;
}

Mesh *LoadOBJFile(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("LoadOBJFile: Unable to load file %s\n", filename);
		return NULL;
	}

	OBJ *obj = InitializeOBJ(OBJ_ARRAY_DEFAULT_CAPACITY);

	int32_t c;
	while((c = fgetc(file)) != EOF) {
		switch (c) {
			case '#':
				ReadComment(file);
				break;
			case 'v':
				c = fgetc(file);

				/* don't read info for vt... etc */
				if (isspace(c))
					ReadVertex(file, obj);
				break;
			case 'f':
				ReadFace(file, obj);
				break;
		}
	}

	// TODO data sanity check?

	obj->verts = realloc(obj->verts, obj->vert_count * sizeof(Vertex));
	obj->tris = realloc(obj->tris, obj->tri_count * sizeof(Triangle));

	Mesh *mesh = malloc(sizeof(Mesh));
	mesh->vert_count = obj->vert_count;
	mesh->verts = obj->verts;
	mesh->tri_count = obj->tri_count;
	mesh->tris = obj->tris;

	free(obj);

	return mesh;
}

void MeshFree(Mesh *mesh)
{
	if (mesh->tris != NULL)
		free(mesh->tris);

	if (mesh->verts != NULL)
		free(mesh->verts);

	free(mesh);
}

/* convenience function for getting a vertex from a triangle face */
Vertex GetVertex(Mesh *mesh, Triangle t, uint32_t i)
{
	return mesh->verts[t.vertex_indices[i]];
}
