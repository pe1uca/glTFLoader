#pragma once
#include "Engine.h"
#include "Camera.h"
#include "Types.h"
#include "Shader.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class Game
{
public:
	Game();
	~Game();

	GLboolean init();
	void run();
	void release();

	Engine::EXIT_CODE GetExitValue();

private:
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
	GLdouble currentFrame = 0.0f;

	Engine::EXIT_CODE mExitValue;
	Engine* mEngine;

	glm::mat4 projection;
	glm::mat4 view;
	glTFFile **models, *bamboo;
	glTFFile *selectedItem;
	GLuint modelsCount = 0;
	Shader *basicShader, *simpleShader, *pbrShader;
	GLuint planeVAO, triangleVAO;

	void update();

	void processInput();

	void render();

	Ray CastRay(GLfloat x, GLfloat y);
};