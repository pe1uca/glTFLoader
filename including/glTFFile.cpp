#include "Types.h"
#include <iostream>

void Primitive::setup(Vertex *vertices, GLuint verticesCount, GLuint *indices, GLuint indicesCount, GLuint material)
{
	this->vertices = vertices;
	this->verticesCount = verticesCount;
	this->indices = indices;
	this->indicesCount = indicesCount;
	this->material = material;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticesCount * sizeof(Vertex), this->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicesCount * sizeof(GLuint), this->indices, GL_STATIC_DRAW);
	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord0));
	// Vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// Vertex normals
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}

void Primitive::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, this->indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void glTFFile::draw(GLuint sceneIndex)
{
	if (this->scenesCount <= sceneIndex)
		return;
	
	Scene *scene = &this->scenes[sceneIndex];

	for (GLuint i = 0; i < scene->nodesCount; i++)
	{
		GLuint nodeIndex = scene->nodes[i];
		if (this->nodesCount <= nodeIndex)
			return;
		Node *node = &this->nodes[nodeIndex];
		Mesh *mesh = &this->meshes[node->mesh];
		for (GLuint j = 0; j < mesh->primitivesCount; j++)
		{
			mesh->primitives[j].draw();
		}
	}
}