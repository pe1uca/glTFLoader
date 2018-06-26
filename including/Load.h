#pragma once
#include "Types.h"
#include <string>

class Loader
{
public:

	glTFFile* LoadFile(const char *filePath);

private:
	GLuint GetComponentCount(std::string component)
	{
		if ("SCALAR" == component) return 1;
		if ("VEC2" == component) return 2;
		if ("VEC3" == component) return 3;
		if ("VEC4" == component) return 4;
		if ("MAT2" == component) return 4;
		if ("MAT3" == component) return 9;
		if ("MAT4" == component) return 16;
		return 0;
	}

	GLuint GetComponentSize(GLuint type)
	{
		switch (type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			return 2;
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
			return 4;
		default:
			return 0;
			break;
		}
	}
};