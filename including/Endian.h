#pragma once
#include <glad/glad.h>

class Endian
{
public:

	GLshort(*bigShort) (GLshort s);
	GLshort(*littleShort) (GLshort s);
	GLint(*bigInt) (GLint i);
	GLint(*littleInt) (GLint i);
	GLfloat(*bigFloat) (GLfloat f);
	GLfloat(*littleFloat) (GLfloat f);

	void Init();
	GLboolean IsBigEndian() { return this->mIsBigEndian; }
private:
	GLboolean mIsBigEndian;
};
