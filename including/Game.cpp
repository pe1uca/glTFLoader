#include "Game.h"

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

	this->bamboo = this->mEngine->mLoader->LoadFile("D:\\etc\\naturekit\\Models\\glTF format\\bamboo.gltf");
	basicShader = new Shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
	simpleShader = new Shader("resources/shaders/simple.vs", "resources/shaders/simple.fs");

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
	
	projection = glm::perspective(glm::radians(this->mEngine->GetCamera()->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	view = this->mEngine->GetCamera()->GetViewMatrix();
}

void Game::render()
{
	glm::mat4 model;
	//model = glm::scale(model, glm::vec3(1.0f));
	basicShader->use();
	basicShader->setMat4("projection", this->projection);
	basicShader->setMat4("view", this->view);
	basicShader->setMat4("model", model);
	this->bamboo->draw(0);

	glBindVertexArray(planeVAO);
	basicShader->setMat4("model", glm::mat4());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	this->mEngine->render();
}

void Game::release()
{

	delete this->basicShader;
	delete this->bamboo;
	this->mEngine->release();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput()
{

}
