#include "Ray.h"

GLbyte Ray::GetSign(GLfloat value)
{
	return 0.0f == value ? 0x01 : value < 0.0f ? 0x02 : 0x00;
}

RayType Ray::GetType()
{
	GLbyte x = GetSign(direction.x);
	GLbyte y = GetSign(direction.y);
	GLbyte z = GetSign(direction.z);
	return (RayType)((x << 4) | (y << 2) | z);
}