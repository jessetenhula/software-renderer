#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define DEFINE_LIST(TYPE, SHORTHAND)\
typedef struct {\
	size_t count;\
	size_t capacity;\
	TYPE *items;\
} SHORTHAND##List;\
\
SHORTHAND##List *SHORTHAND##List_Init(size_t capacity)\
{\
	SHORTHAND##List *l = malloc(sizeof(SHORTHAND##List));\
\
	l->count = 0;\
	l->capacity = capacity;\
\
	l->items = malloc(capacity * sizeof(TYPE));\
\
	return l;\
}\
\
void SHORTHAND##List_Free(SHORTHAND##List *l)\
{\
	free(l->items);\
	free(l);\
}\
\
void SHORTHAND##List_Put(SHORTHAND##List *l, TYPE item)\
{\
	if (l->count == l->capacity) {\
		size_t new_capacity = l->capacity * 2;\
		if (new_capacity < l->capacity) {\
			printf("Coulnd't allocate more memory for list items\n");\
			return;\
		}\
\
		l->items = realloc(l->items, new_capacity * sizeof(TYPE));\
		l->capacity = new_capacity;\
	}\
\
	l->items[l->count++] = item;\
}\
\
TYPE SHORTHAND##List_Get(SHORTHAND##List *l, size_t i)\
{\
	if (i >= l->count) {\
		printf("Index out of bounds for list\n");\
		TYPE placeholder = { 0 }; \
		return placeholder;\
	}\
\
	return l->items[i];\
}

#endif /* UTIL_H */
