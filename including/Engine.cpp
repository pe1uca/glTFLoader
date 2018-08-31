#include "Engine.h"
#include "Tools.h"
#include <iostream>
#include <map>

Engine::Engine()
{
}

Engine::~Engine()
{
}

ErrorCalls Engine::init(void* /*_init*/)
{
	if (!this->initiWindow())
	{
		return ErrorCalls::FAILURE;
	}

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return ErrorCalls::FAILURE;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	this->mLoader = new Loader();
	this->mCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	ConsoleOn = false;
	//mConsoleBuffer = "";
	return ErrorCalls::SUCCESS;
}

void Engine::release()
{
	FREE_MEMORY(mLoader);
	FREE_MEMORY(mCamera);
	glfwTerminate();
}

GLboolean Engine::initiWindow()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	this->mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Including", NULL, NULL);
	if (this->mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return GL_FALSE;
	}
	glfwMakeContextCurrent(this->mWindow);
	glfwSetWindowUserPointer(this->mWindow, this);

	glfwSetFramebufferSizeCallback(this->mWindow, [](GLFWwindow* window, int width, int height)
	{
		((Engine*)(glfwGetWindowUserPointer(window)))->framebuffer_size_callback(window, width, height);
	});
	glfwSetCursorPosCallback(this->mWindow, [](GLFWwindow* window, double xpos, double ypos)
	{
		((Engine*)(glfwGetWindowUserPointer(window)))->mouse_callback(window, xpos, ypos);
	});
	glfwSetScrollCallback(this->mWindow, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		((Engine*)(glfwGetWindowUserPointer(window)))->scroll_callback(window, xoffset, yoffset);
	});

	glfwSetKeyCallback(this->mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		((Engine*)(glfwGetWindowUserPointer(window)))->key_callback(window, key, scancode, action, mods);
	});
	glfwSetInputMode(this->mWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_PRESS);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(this->mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return GL_TRUE;
}
void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ConsoleOn)
	{
		if (keyPressed(key))
		{
			if (key == GLFW_KEY_ENTER)
			{
				mConsoleBuffer = "\n";
				return;
			}
			if (key == GLFW_KEY_BACKSPACE)
			{
				if (!mConsoleBuffer.empty())
				{
					mConsoleBuffer.pop_back();
					return;
				}
			}
			if (key == GLFW_KEY_ESCAPE)
				ConsoleOn = false;
			mConsoleBuffer.push_back((char)(key));
		}
	}
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Engine::framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	this->SCR_WIDTH = width;
	this->SCR_HEIGHT = height;
	this->aspectRatio = (GLfloat)width / (GLfloat)height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		if (firstMouse)
		{
			lastX = (float)xpos;
			lastY = (float)ypos;
			firstMouse = false;
		}

		float xoffset = (float)xpos - lastX;
		float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

		lastX = (float)xpos;
		lastY = (float)ypos;

		mCamera->ProcessMouseRotation(xoffset, yoffset);
	}
	else if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (firstMouse)
		{
			lastX = (float)xpos;
			lastY = (float)ypos;
			firstMouse = false;
		}

		float xoffset = (float)xpos - lastX;
		float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

		lastX = (float)xpos;
		lastY = (float)ypos;

		mCamera->ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		firstMouse = true;
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Engine::scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
	mCamera->ProcessMouseScroll((float)yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Engine::processInput(GLfloat deltaTime)
{
	if (keyPressed(GLFW_KEY_ESCAPE))
	{
		if (!ConsoleOn)
			glfwSetWindowShouldClose(this->mWindow, true);
	}
	if (keyPressed(GLFW_KEY_ENTER) && !ConsoleOn)
	{
		std::cout << "Consola prendida \n";
		ConsoleOn = true;
	}
	if (!ConsoleOn)
	{
		if (glfwGetKey(this->mWindow, GLFW_KEY_W) == GLFW_PRESS)
			mCamera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(this->mWindow, GLFW_KEY_S) == GLFW_PRESS)
			mCamera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(this->mWindow, GLFW_KEY_A) == GLFW_PRESS)
			mCamera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(this->mWindow, GLFW_KEY_D) == GLFW_PRESS)
			mCamera->ProcessKeyboard(RIGHT, deltaTime);
	}
}

GLboolean Engine::keyPressed(int key)
{
	if (GLFW_PRESS == glfwGetKey(this->mWindow, key) && GLFW_RELEASE == keysPrevState[key])
	{
		keysPrevState[key] = GLFW_PRESS;
		return true;
	}
	else if (GLFW_RELEASE == glfwGetKey(this->mWindow, key))
	{
		keysPrevState[key] = GLFW_RELEASE;
	}
	return false;
}

GLboolean Engine::mouseButtonPressed(int button)
{
	if (GLFW_PRESS == glfwGetMouseButton(this->mWindow, button) && GLFW_RELEASE == mousePrevState[button])
	{
		mousePrevState[button] = GLFW_PRESS;
		return true;
	}
	else if (GLFW_RELEASE == glfwGetMouseButton(this->mWindow, button))
	{
		mousePrevState[button] = GLFW_RELEASE;
	}
	return false;
}

GLboolean Engine::keyDown(int key)
{
	return GLFW_PRESS == glfwGetKey(this->mWindow, key);
}

GLboolean Engine::GetShouldClose()
{
	return (GLboolean)glfwWindowShouldClose(this->mWindow);
}
void Engine::SetShouldClose(GLboolean value)
{
	glfwSetWindowShouldClose(this->mWindow, value);
}

void Engine::update(GLfloat deltaTime)
{
	this->processInput(deltaTime);
	if (!mConsoleBuffer.empty())
	{
		std::cout << mConsoleBuffer;
		mConsoleBuffer.clear();
	}
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::render()
{
	glfwSwapBuffers(this->mWindow);
	glfwPollEvents();
	
}

GLint Engine::registerBoundingBox(Box box)
{
	objectsToColide.push_back(box);
	return objectsToColide.size() - 1;
}

GLint Engine::CheckCollision(Ray ray)
{
	std::map<float, GLint> sorted;

	for (GLuint i = 0; i < objectsToColide.size(); i++)
	{
		GLfloat distance = glm::length(this->mCamera->Position - ((objectsToColide[i].bounds[0] + objectsToColide[i].bounds[1]) / 2.0f));
		sorted[distance] = i;
	}

	for (std::map<float, GLint>::iterator it = sorted.begin(); it != sorted.end(); ++it)
	{
		Box test = objectsToColide[it->second];
		GLboolean collision = test.interect(ray);
		if (collision)
		{
			return it->second;
		}
	}

	return -1;
}