#include "objfile.h"

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

/* return END_OF_LINE (-1) if at end of line or file, otherwise return 0 */
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
	unsigned char token[TOKEN_MAX_LENGTH];

	Vertex v = { 0, 0, 0, 1 };

	ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.x = atof(token);

	ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.y = atof(token);

	int32_t return_code = ReadToken(file, token, TOKEN_MAX_LENGTH);
	v.z = atof(token);

	if (return_code != END_OF_LINE) {
		ReadToken(file, token, TOKEN_MAX_LENGTH);
		v.w = atof(token);
	}

	/* list put */
	if (obj->v_count == obj->v_capacity) {
		uint32_t new_capacity = obj->v_capacity * 2;
		obj->vs = realloc(obj->vs, new_capacity * sizeof(Vertex));
		obj->v_capacity = new_capacity;
	}

	obj->vs[obj->v_count++] = v;
	/* list put */
}

static void ReadFace(FILE *file, OBJ *obj)
{
	unsigned char token[TOKEN_MAX_LENGTH];

	Face f;
	f.count = 0;
	f.v_is = malloc(FACE_MAX_VERTICES * sizeof(*(f.v_is)));

	int32_t return_code = 0;

	do {
		return_code = ReadToken(file, token, TOKEN_MAX_LENGTH);
		/* only reads the regular vertex index (v) */
		f.v_is[f.count++] = atoi(token);
	} while (return_code != END_OF_LINE && f.count < FACE_MAX_VERTICES);

	/* list put */
	if (obj->f_count == obj->f_capacity) {
		uint32_t new_capacity = obj->f_capacity * 2;
		obj->fs = realloc(obj->fs, new_capacity * sizeof(Face));
		obj->f_capacity = new_capacity;
	}

	obj->fs[obj->f_count++] = f;
	/* list put */
}

OBJ *LoadOBJFile(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("LoadOBJFile: Unable to load file %s\n", filename);
		return NULL;
	}

	int32_t c;

	OBJ *obj = malloc(sizeof(OBJ));;
	obj->v_capacity = INITIAL_VERTEX_CAPACITY;
	obj->f_capacity = INITIAL_FACE_CAPACITY;
	obj->vs = malloc(obj->v_capacity * sizeof(Vertex));
	obj->fs = malloc(obj->f_capacity * sizeof(Face));

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

	if (obj->v_capacity > obj->v_count)
		obj->vs = realloc(obj->vs, obj->v_count * sizeof(Vertex));

	if (obj->f_capacity > obj->f_count)
		obj->fs = realloc(obj->fs, obj->f_count * sizeof(Face));

	return obj;
}

