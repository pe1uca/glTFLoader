#pragma once
#include "Load.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include "Camera.h"
#include "Module.h"
/*Program*/
class Engine : public Module<Engine>
{
public:

	enum EXIT_CODE
	{
		FAILURE = -1,
		SUCCESS
	};

	Loader* mLoader;

	Engine();
	~Engine();

	ErrorCalls init(void* _init);
	void release();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLboolean keyPressed(int key);
	GLboolean keyDown(int key);
	GLboolean mouseButtonPressed(int button);
	GLint registerBoundingBox(Box box);
	GLint CheckCollision(Ray ray);

	void update(GLfloat deltaTime);

	void render();

	GLboolean GetShouldClose();
	void SetShouldClose(GLboolean value);
	Camera *GetCamera() { return this->mCamera; }
	void GetCursosPos(GLdouble *xpos, GLdouble *ypos) { glfwGetCursorPos(this->mWindow, xpos, ypos); }
	GLint GetMouseButtonState(GLint button) { return glfwGetMouseButton(this->mWindow, button); }
	GLuint GetWindowWidth() { return this->SCR_WIDTH; }
	GLuint GetWindowHeight() { return this->SCR_HEIGHT; }
	GLfloat GetFarPlane() { return this->farPlane; }
	GLfloat GetNearPlane() { return this->nearPlane; }
	GLfloat GetAspectRatio() { return this->aspectRatio; }

private:
	GLFWwindow* mWindow;
	GLuint SCR_WIDTH = 1280;
	GLuint SCR_HEIGHT = 720;
	GLfloat nearPlane = 0.1f;
	GLfloat farPlane = 100.0f;
	GLfloat aspectRatio = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
	float lastX = (float)SCR_WIDTH / 2.0f;
	float lastY = (float)SCR_HEIGHT / 2.0f;
	GLboolean keysPrevState[GLFW_KEY_LAST];
	GLboolean mousePrevState[GLFW_MOUSE_BUTTON_LAST];
	bool firstMouse = true;
	Camera *mCamera;
	std::vector<Box> objectsToColide;

	GLboolean initiWindow();

	void processInput(GLfloat deltaTime);

};
