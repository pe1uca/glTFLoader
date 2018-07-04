#pragma once
#include <glm\glm.hpp>
#include <glad\glad.h>

#include "Ray.h"

class Box
{
public:
	glm::vec3 bounds[2];
	Box()
	{
		bounds[0] = glm::vec3(std::numeric_limits<GLfloat>::max());
		bounds[1] = glm::vec3(-std::numeric_limits<GLfloat>::max());
	}
	~Box() {}

	GLboolean interect(Ray ray) const;
};