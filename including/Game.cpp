#include "Game.h"
#include "dirent.h"

#include <iostream>

float planeVertices[] = {
	// positions			//Normals		// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	-5.0f, -0.5f, -5.0f,	0.0, 1.0, 0.0,	0.0f, 2.0f,
	-5.0f, -0.5f,  5.0f,	0.0, 1.0, 0.0,	0.0f, 0.0f,
	5.0f, -0.5f,  5.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f,

	5.0f, -0.5f, -5.0f,		0.0, 1.0, 0.0,	2.0f, 2.0f,
	-5.0f, -0.5f, -5.0f,	0.0, 1.0, 0.0,	0.0f, 2.0f,
	5.0f, -0.5f,  5.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f
};

float triangleVertices[] = {
	// positions			//Normals		// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,	0.0f, 2.0f,
	0.5f, 0.0f,  0.0f,		0.0, 1.0, 0.0,	0.0f, 0.0f,
	-0.5f, 0.0f,  0.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f
};

glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f)
};

Game::Game() :
	mExitValue(Engine::SUCCESS)
{
}

Game::~Game()
{
}

Engine::EXIT_CODE Game::GetExitValue() { return this->mExitValue; }

GLboolean Game::init()
{
	this->mEngine = new Engine();
	if (!this->mEngine->init())
	{
		this->mExitValue = Engine::FAILURE;
		return GL_FALSE;
	}
	this->bamboo = this->mEngine->mLoader->LoadFile("D:\\etc\\naturekit\\Models\\glTF format\\palmDetailed_large.gltf");
	/*struct dirent **dirp;
	modelsCount = scandir("D:\\etc\\naturekit\\Models\\glTF format\\", &dirp, [](const struct dirent *dir) 
	{
		const char *s = dir->d_name;
		int len = strlen(s) - 5; // index of start of . in .mp3
		if (len >= 0)
		{
			if (strncmp(s + len, ".gltf", 5) == 0)
			{
				return 1;
			}
		}
		return 0;
	}, alphasort);
	if (modelsCount > 0)
	{
		this->models = new glTFFile*[modelsCount];
		for (GLuint i = 0; i < modelsCount; i++)
		{
			std::cout << dirp[i]->d_name << std::endl;
			if (i == 150)
				std::cout << "to crash" << std::endl;
			this->models[i] = this->mEngine->mLoader->LoadFile(std::string("D:\\etc\\naturekit\\Models\\glTF format\\").append(dirp[i]->d_name).c_str());
		}
	}*/
	//this->bamboo = this->mEngine->mLoader->LoadFile("resources\\models\\bamboo.gltf");
	for (GLint i = 0; i < this->bamboo->nodesCount; i++)
	{
		this->mEngine->registerBoundingBox(this->bamboo->nodes[i].boundingBox);
	}
	basicShader = new Shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
	simpleShader = new Shader("resources/shaders/simple.vs", "resources/shaders/simple.fs");
	pbrShader = new Shader("resources/shaders/PBR.vs", "resources/shaders/PBR.fs");

	pbrShader->use();
	for (GLuint i = 0; i < 4; i++)
	{
		pbrShader->setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
		pbrShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}

	GLuint planeVBO, triangleVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
	
	projection = glm::perspective(glm::radians(this->mEngine->GetCamera()->Zoom), this->mEngine->GetAspectRatio(), this->mEngine->GetNearPlane(), this->mEngine->GetFarPlane());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	return GL_TRUE;
}

void Game::run()
{
	while (!this->mEngine->GetShouldClose())
	{
		this->currentFrame = glfwGetTime();
		this->deltaTime = this->currentFrame - this->lastFrame;
		this->lastFrame = this->currentFrame;

		this->processInput();

		this->update();

		this->render();
	}
}

void Game::update()
{
	this->mEngine->update(this->deltaTime);
	projection = glm::perspective(glm::radians(this->mEngine->GetCamera()->Zoom), this->mEngine->GetAspectRatio(), this->mEngine->GetNearPlane(), this->mEngine->GetFarPlane());
	view = this->mEngine->GetCamera()->GetViewMatrix();
}

void Game::render()
{
	glm::mat4 model;
	//model = glm::scale(model, glm::vec3(1.0f));
	basicShader->use();
	basicShader->setMat4("projection", this->projection);
	basicShader->setMat4("view", this->view);
	//this->bamboo->draw(0, *basicShader);

	pbrShader->use();
	pbrShader->setMat4("projection", this->projection);
	pbrShader->setMat4("view", this->view);
	pbrShader->setVec3("camPos", this->mEngine->GetCamera()->Position);
	this->bamboo->draw(0, pbrShader);
	for (GLuint i = 0; i < modelsCount; i++)
	{
		this->models[i]->draw(0, pbrShader);
	}

	glBindVertexArray(planeVAO);
	basicShader->use();
	basicShader->setMat4("model", glm::mat4());
	basicShader->setVec4("baseColor", glm::vec4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	this->mEngine->render();
}

void Game::release()
{

	delete this->basicShader;
	delete this->bamboo;
	//delete this->models;
	this->mEngine->release();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput()
{
	if (this->mEngine->mouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		GLdouble x, y;
		this->mEngine->GetCursosPos(&x, &y);

		Ray ray = this->CastRay(x, y);

		GLint index = this->mEngine->CheckCollision(ray);

		std::cout << index << std::endl;
	}
}

Ray Game::CastRay(GLfloat x, GLfloat y)
{
	Line line;

	GLfloat tanV = glm::tan(this->mEngine->GetCamera()->Zoom * 0.5f);

	GLfloat dx = tanV * (x / (this->mEngine->GetWindowWidth() / 2) - 1.0f) / this->mEngine->GetAspectRatio();
	GLfloat dy = tanV * (1.0f - y / (this->mEngine->GetWindowHeight() / 2));
	GLfloat nearPlane = this->mEngine->GetNearPlane();
	GLfloat farPlane = this->mEngine->GetFarPlane();

	line.start = glm::vec3(dx * nearPlane, dy * nearPlane, nearPlane);
	line.end = glm::vec3(dx * farPlane, dy * farPlane, farPlane);
	
	glm::mat4 inverseView = glm::inverse(this->view);

	line.start = inverseView * glm::vec4(line.start, 1.0);
	line.end = inverseView * glm::vec4(line.end, 1.0);

	Ray result;
	result.origin = line.start;
	result.direction = line.end - line.start;

	return result;
}
