#include "Endian.h"

GLshort shortSwap(GLshort value)
{
	return ((value & 0xFF) << 8) + ((value >> 8) & 0xFF);
}

inline GLshort shortNoSwap(GLshort value) { return value; }

GLint intSwap(GLint value)
{
	return ((int)((value & 0xFF) << 24) +
			(int)(((value >> 8) & 0xFF) << 16) +
			(int)(((value >> 16) & 0xFF) << 8) +
			(int)((value >> 24) & 0xFF));
}

inline GLint intNoSwap(GLint value) { return value; }

GLfloat floatSwap(GLfloat value)
{
	union 
	{
		GLfloat f;
		GLubyte b[4];
	} tmp, result;

	tmp.f = value;
	result.b[0] = tmp.b[3];
	result.b[1] = tmp.b[2];
	result.b[2] = tmp.b[1];
	result.b[3] = tmp.b[0];
	return result.f;
}

inline GLfloat floatNoSwap(GLfloat value) { return value; }

void Endian::Init()
{
	GLbyte endianTest[2] = { 0, 1 };
	this->mIsBigEndian = 0 == *(GLshort*)endianTest;
	if (this->mIsBigEndian)
	{
		this->bigShort		= shortNoSwap;
		this->littleShort	= shortSwap;
		this->bigInt		= intNoSwap;
		this->littleInt		= intSwap;
		this->bigFloat		= floatNoSwap;
		this->littleFloat	= floatSwap;
	}
	else
	{
		this->bigShort		= shortSwap;
		this->littleShort	= shortNoSwap;
		this->bigInt		= intSwap;
		this->littleInt		= intNoSwap;
		this->bigFloat		= floatSwap;
		this->littleFloat	= floatNoSwap;
	}
}