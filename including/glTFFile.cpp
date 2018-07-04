#include "Types.h"
#include <glm\gtc\matrix_transform.hpp>
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

void glTFFile::draw(GLuint sceneIndex, Shader *shader)
{
	if (this->scenesCount <= sceneIndex)
		return;
	
	Scene *scene = &this->scenes[sceneIndex];

	for (GLuint i = 0; i < scene->nodesCount; i++)
	{
		GLuint nodeIndex = scene->nodes[i];
		if (this->nodesCount <= nodeIndex)
			continue;
		this->drawNode(nodeIndex, glm::mat4(), shader);
	}
}

void glTFFile::drawNode(GLuint index, glm::mat4 parentM, Shader *shader)
{
	Node *node = &this->nodes[index];
	glm::mat4 model;
	model = glm::translate(model, node->translation);
	model = glm::scale(model, node->scale);
	model = parentM * model;

	if (node->hasMesh)
	{
		Mesh *mesh = &this->meshes[node->mesh];
		shader->setMat4("model", model);
		for (GLuint j = 0; j < mesh->primitivesCount; j++)
		{
			Material *material = &this->materials[mesh->primitives[j].material];
			shader->setVec4("baseColorFactor", material->color);
			shader->setFloat("metallic", material->metallic);
			shader->setFloat("roughness", 1.0f);
			shader->setFloat("ao", 1.0f);
			shader->setVec3("albedo", glm::vec3(1.0f));
			mesh->primitives[j].draw();
		}
	}

	if (nullptr != node->children)
	{
		for (GLuint i = 0; i < node->childrenCount; i++)
		{
			GLuint nodeIndex = node->children[i];
			if (this->nodesCount <= nodeIndex)
				continue;
			this->drawNode(nodeIndex, model, shader);
		}
	}
}

void glTFFile::setup()
{

	for (GLint i = 0; i < this->nodesCount; i++)
	{
		if (!this->nodes[i].isRoot)
			continue;
		Node *node = &this->nodes[i];
		glm::mat4 model;
		model = glm::translate(model, node->translation);
		model = glm::scale(model, node->scale);
		model = model;

		node->boundingBox = calculateBoundingBox(i, glm::mat4());
	}
}

Box glTFFile::calculateBoundingBox(GLuint index, glm::mat4 parentModel)
{
	Box result;
	Node *node = &this->nodes[index];
	glm::mat4 model;
	model = glm::translate(model, node->translation);
	model = glm::scale(model, node->scale);
	model = parentModel * model;
	if (nullptr != node->children)
	{
		for (GLint i = 0; i < node->childrenCount; i++)
		{
			Box child = calculateBoundingBox(node->children[i], model);
			result.bounds[1].x = child.bounds[1].x > result.bounds[1].x ? child.bounds[1].x : result.bounds[1].x;
			result.bounds[1].y = child.bounds[1].y > result.bounds[1].y ? child.bounds[1].y : result.bounds[1].y;
			result.bounds[1].z = child.bounds[1].z > result.bounds[1].z ? child.bounds[1].z : result.bounds[1].z;

			result.bounds[0].x = child.bounds[0].x < result.bounds[0].x ? child.bounds[0].x : result.bounds[0].x;
			result.bounds[0].y = child.bounds[0].y < result.bounds[0].y ? child.bounds[0].y : result.bounds[0].y;
			result.bounds[0].z = child.bounds[0].z < result.bounds[0].z ? child.bounds[0].z : result.bounds[0].z;
		}
	}
	if (node->hasMesh)
	{
		Box child = node->boundingBox;

		child.bounds[1] = model * glm::vec4(child.bounds[1], 1.0);
		child.bounds[0] = model * glm::vec4(child.bounds[0], 1.0);

		result.bounds[1].x = child.bounds[1].x > result.bounds[1].x ? child.bounds[1].x : result.bounds[1].x;
		result.bounds[1].y = child.bounds[1].y > result.bounds[1].y ? child.bounds[1].y : result.bounds[1].y;
		result.bounds[1].z = child.bounds[1].z > result.bounds[1].z ? child.bounds[1].z : result.bounds[1].z;

		result.bounds[0].x = child.bounds[0].x < result.bounds[0].x ? child.bounds[0].x : result.bounds[0].x;
		result.bounds[0].y = child.bounds[0].y < result.bounds[0].y ? child.bounds[0].y : result.bounds[0].y;
		result.bounds[0].z = child.bounds[0].z < result.bounds[0].z ? child.bounds[0].z : result.bounds[0].z;
		node->boundingBox = child;
	}
	return result;
}