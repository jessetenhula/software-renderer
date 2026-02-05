#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>

typedef struct {
	double x, y, z, w;
} Vec4;

typedef struct {
	double x, y, z;
} Vec3;

typedef struct {
	double x, y;
} Vec2;

typedef struct {
	uint16_t width;
	uint16_t height;
	double *data;
} Matrix;

Vec2 Vec2Add(Vec2 a, Vec2 b);
Vec2 Vec2Sub(Vec2 a, Vec2 b);
Vec2 Vec2Dot(Vec2 a, Vec2 b);

Vec3 Vec3Add(Vec3 a, Vec3 b);
Vec3 Vec3Sub(Vec3 a, Vec3 b);
Vec3 Vec3Dot(Vec3 a, Vec3 b);
Vec3 Vec3Cross(Vec3 a, Vec3 b);

Vec4 Vec4Add(Vec4 a, Vec4 b);
Vec4 Vec4Sub(Vec4 a, Vec4 b);
Vec4 Vec4Dot(Vec4 a, Vec4 b);

#endif /* GEOMETRY_H */
