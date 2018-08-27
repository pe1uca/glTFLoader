#pragma once
#include <glm\glm.hpp>
#include <glad\glad.h>

enum RayType
{
	// Bits: (1 = negative, 0 = positive)
	// x sign, x zero, y sign, y zero, z sign, z zero
	// M: 10	P: 00	O: 01
	MMM = 0x2A, //b00101010
	MMP = 0x28, //b00101000
	MPM = 0x22, //b00100010
	MPP = 0x20, //b00100000
	PMM = 0x0A, //b00001010
	PMP = 0x08, //b00001000
	PPM = 0x02, //b00000010
	PPP = 0x00, //b00000000
	MOO = 0x25, //b00100101
	POO = 0x05, //b00000101
	OMO = 0x19, //b00011001
	OPO = 0x11, //b00010001
	OOM = 0x16, //b00010110
	OOP = 0x14, //b00010100
	MMO = 0x29, //b00101001
	MPO = 0x21, //b00100001
	PMO = 0x09, //b00001001
	PPO = 0x01, //b00000001
	MOM = 0x26, //b00100110
	MOP = 0x24, //b00100100
	POM = 0x06, //b00000110
	POP = 0x04, //b00000100
	OMM = 0x1A, //b00011010
	OMP = 0x18, //b00011000
	OPM = 0x12, //b00010010
	OPP = 0x10  //b00010000
};

class Ray
{
public:
	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 inverseDir;
	RayType type;
	GLfloat sYX, cYX;
	GLfloat sXY, cXY;
	GLfloat sZY, cZY;
	GLfloat sYZ, cYZ;
	GLfloat sXZ, cXZ;
	GLfloat sZX, cZX;
	GLint sign[3];
	Ray() {}
	Ray(glm::vec3 origin, glm::vec3 direction) {
		this->origin = origin;
		this->direction = glm::normalize(direction);
		inverseDir.x = 1.0f / direction.x;
		inverseDir.y = 1.0f / direction.y;
		inverseDir.z = 1.0f / direction.z;

		type = GetType();

		sYX = direction.x * inverseDir.y;
		sXY = direction.y * inverseDir.x;
		sZY = direction.y * inverseDir.z;
		sYZ = direction.z * inverseDir.y;
		sXZ = direction.z * inverseDir.x;
		sZX = direction.x * inverseDir.z;

		cYX = origin.x - sYX * origin.y;
		cXY = origin.y - sXY * origin.x;
		cZY = origin.y - sZY * origin.z;
		cYZ = origin.z - sYZ * origin.y;
		cXZ = origin.z - sXZ * origin.x;
		cZX = origin.x - sZX * origin.z;
	}
	~Ray() {}
private:
	RayType GetType();
	GLbyte GetSign(GLfloat value);
};
