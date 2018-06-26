#pragma once
#include "Load.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "Camera.h"

class Engine
{
public:

	enum EXIT_CODE
	{
		FAILURE = -1,
		SUCCESS
	};

	const GLuint SCR_WIDTH = 1280;
	const GLuint SCR_HEIGHT = 720;

	Loader* mLoader;

	Engine();
	~Engine();

	GLboolean init();
	void release();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLboolean keyPressed(int key);
	GLboolean keyDown(int key);

	void update(GLfloat deltaTime);

	void render();

	GLboolean GetShouldClose();
	void SetShouldClose(GLboolean value);
	Camera *GetCamera() { return this->mCamera; }

private:
	GLFWwindow* mWindow;
	float lastX = (float)SCR_WIDTH / 2.0;
	float lastY = (float)SCR_HEIGHT / 2.0;
	GLboolean keysPrevState[GLFW_KEY_LAST];
	bool firstMouse = true;
	Camera *mCamera;

	GLboolean initiWindow();

	void processInput(GLfloat deltaTime);

};
