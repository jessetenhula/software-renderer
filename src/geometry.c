#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "geometry.h"

/* Vec2 */

Vec2 Vec2Add(Vec2 a, Vec2 b)
{
	return (Vec2) { a.x + b.x, a.y + b.y };
}

Vec2 Vec2Sub(Vec2 a, Vec2 b)
{
	return (Vec2) { a.x - b.x, a.y - b.y };
}

double Vec2Dot(Vec2 a, Vec2 b)
{
	return (a.x * b.x) + (a.y + b.y);
}

/* Vec3 */

Vec3 Vec3Add(Vec3 a, Vec3 b)
{
	return (Vec3) { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 Vec3Sub(Vec3 a, Vec3 b)
{
	return (Vec3) { a.x - b.x, a.y - b.y, a.z - b.z };
}

double Vec3Dot(Vec3 a, Vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 Vec3Cross(Vec3 a, Vec3 b)
{
	return (Vec3) {
		a.y * b.z - b.y * a.z,
		a.z * b.x - b.z * a.x,
		a.x * b.y - b.x * a.y,
	};
}

/* Vec4 */

Vec4 Vec4Add(Vec4 a, Vec4 b)
{
	return (Vec4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vec4 Vec4Sub(Vec4 a, Vec4 b)
{
	return (Vec4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

double Vec4Dot(Vec4 a, Vec4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w + b.w);
}

/* Matrix */
