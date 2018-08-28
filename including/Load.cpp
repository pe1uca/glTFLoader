#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#include <rapidjson\document.h>
#include <rapidjson\error\en.h>

#include "Load.h"
#include "Endian.h"

glTFFile* Loader::LoadFile(const char *filePath)
{
	glTFFile* result = new glTFFile;
	Endian endian;
	Buffer* buffers;
	BufferView* views;
	Accessor* accessors;
	Material *materials;
	GLuint buffersCount, viewsCount, accessorsCount;
	rapidjson::Document json;
	std::ifstream fileStream;
	std::string file, line, fileDir;

	endian.Init();
	fileDir = filePath;
	fileDir = fileDir.substr(0, fileDir.find_last_of('\\'));

	fileStream.open(filePath);
	while (std::getline(fileStream, line))
	{
		file += line;
		file.push_back('\n');
	}
	fileStream.close();

	json.Parse(file.c_str());
	if (json.HasParseError())
	{
		std::cout << "LOADER::GLTF::PARSER_ERROR Message: " << rapidjson::GetParseError_En(json.GetParseError()) << std::endl;
		return result;
	}

	if (!json.HasMember("asset"))
	{
		std::cout << "LOADER::GLTF::GRAMMAR_ERROR Message: Could not find asset node." << std::endl;
		return result;
	}
	
	rapidjson::Value& value = json["asset"];
	if (!value.HasMember("version"))
	{
		std::cout << "LOADER::GLTF::GRAMMAR_ERROR Message: Could not find asset.version node." << std::endl;
		return result;
	}
	
	std::string version = value["version"].GetString();
	std::string major, minor;
	major = version.substr(0, 1);
	minor = version.substr(version.find('.') + 1, 1);

	if ("2" != major)
	{
		std::cout << "LOADER::GLTF::VERSION Message: Version not supported" << std::endl;
		return result;
	}

	if (!json.HasMember("buffers") || !json["buffers"].IsArray() || json["buffers"].Empty())
	{
		std::cout << "LOADER::GLTF::BUFFERS Message: Could not find buffers array." << std::endl;
		return result;
	}
	if (!json.HasMember("bufferViews") || !json["bufferViews"].IsArray() || json["bufferViews"].Empty())
	{
		std::cout << "LOADER::GLTF::BUFFER_VIEWS Message: Could not find buffer views array." << std::endl;
		return result;
	}
	if (!json.HasMember("meshes") || !json["meshes"].IsArray() || json["meshes"].Empty())
	{
		std::cout << "LOADER::GLTF::MESHES Message: Could not find meshes array." << std::endl;
		return result;
	}
	if (!json.HasMember("accessors") || !json["accessors"].IsArray() || json["accessors"].Empty())
	{
		std::cout << "LOADER::GLTF::MESHES Message: Could not find meshes array." << std::endl;
		return result;
	}
	if (!json.HasMember("nodes") || !json["nodes"].IsArray() || json["nodes"].Empty())
	{
		std::cout << "LOADER::GLTF::NODES Message: Could not find nodes array." << std::endl;
		return result;
	}
	if (!json.HasMember("scenes") || !json["scenes"].IsArray() || json["scenes"].Empty())
	{
		std::cout << "LOADER::GLTF::SCENES Message: Could not find scenes array." << std::endl;
		return result;
	}

	value = json["buffers"];
	buffersCount = value.Size();
	buffers = new Buffer[buffersCount];
	for (GLuint i = 0; i < buffersCount; i++)
	{
		GLuint size = value[i]["byteLength"].GetUint();
		fileStream.open(fileDir + "\\" + value[i]["uri"].GetString(), std::ios::in | std::ios::binary);
		buffers[i].data = new GLubyte[size];
		buffers[i].size = size;

		fileStream.read((char *)buffers[i].data, buffers[i].size);

		fileStream.close();
	}

	value = json["bufferViews"];
	viewsCount = value.Size();
	views = new BufferView[viewsCount];
	for (GLuint i = 0; i < viewsCount; i++)
	{
		views[i].buffer = value[i]["buffer"].GetUint();
		views[i].size = value[i]["byteLength"].GetUint();
		views[i].offset = value[i].HasMember("byteOffset") ? value[i]["byteOffset"].GetUint() : 0;
	}

	value = json["accessors"];
	accessorsCount = value.Size();
	accessors = new Accessor[accessorsCount];
	for (GLuint i = 0; i < accessorsCount; i++)
	{
		accessors[i].view = value[i]["bufferView"].GetUint();
		accessors[i].componentType = value[i]["componentType"].GetUint();
		accessors[i].count = value[i]["count"].GetUint();
		accessors[i].type = value[i]["type"].GetString();
		GLuint componentCount = this->GetComponentCount(accessors[i].type);
		GLuint componentSize = this->GetComponentSize(accessors[i].componentType);
		accessors[i].size = componentCount * componentSize;
		accessors[i].min = new GLchar[accessors[i].size];
		accessors[i].max = new GLchar[accessors[i].size];
		for (GLuint j = 0; j < componentCount; j++)
		{
			GLboolean hasMin = value[i].HasMember("min");
			GLboolean hasMax = value[i].HasMember("max");
			rapidjson::Type tmp;
			switch (accessors[i].componentType)
			{
			case GL_BYTE:
				((GLbyte*)accessors[i].min)[j] = hasMin ? (GLbyte)value[i]["min"][j].GetFloat() : 0;
				((GLbyte*)accessors[i].max)[j] = hasMax ? (GLbyte)value[i]["max"][j].GetFloat() : 0;
				break;
			case GL_UNSIGNED_BYTE:
				((GLubyte*)accessors[i].min)[j] = hasMin ? (GLubyte)value[i]["min"][j].GetFloat() : 0;
				((GLubyte*)accessors[i].max)[j] = hasMax ? (GLubyte)value[i]["max"][j].GetFloat() : 0;
				break;
			case GL_SHORT:
				((GLshort*)accessors[i].min)[j] = hasMin ? (GLshort)value[i]["min"][j].GetFloat() : 0;
				((GLshort*)accessors[i].max)[j] = hasMax ? (GLshort)value[i]["max"][j].GetFloat() : 0;
				break;
			case GL_UNSIGNED_SHORT:
				tmp = value[i]["min"][j].GetType();
				((GLushort*)accessors[i].min)[j] = hasMin ? (GLushort)value[i]["min"][j].GetFloat() : 0;
				((GLushort*)accessors[i].max)[j] = hasMax ? (GLushort)value[i]["max"][j].GetFloat() : 0;
				break;
			case GL_UNSIGNED_INT:
				((GLuint*)accessors[i].min)[j] = hasMin ? (GLuint)value[i]["min"][j].GetFloat() : 0;
				((GLuint*)accessors[i].max)[j] = hasMax ? (GLuint)value[i]["max"][j].GetFloat() : 0;
				break;
			case GL_FLOAT:
				((GLfloat*)accessors[i].min)[j] = hasMin ? (GLfloat)value[i]["min"][j].GetFloat() : 0.0f;
				((GLfloat*)accessors[i].max)[j] = hasMax ? (GLfloat)value[i]["max"][j].GetFloat() : 0.0f;
				break;
			}
		}
	}

	value = json["materials"];
	result->materialsCount = value.Size();
	materials = new Material[result->materialsCount];
	result->materials = materials;
	for (GLuint i = 0; i < result->materialsCount; i++)
	{
		materials[i].color = glm::vec4(1.0f);
		if (value[i]["pbrMetallicRoughness"].HasMember("baseColorFactor"))
		{
			rapidjson::Value& baseColor = value[i]["pbrMetallicRoughness"]["baseColorFactor"];
			materials[i].color.r = baseColor[0].GetFloat();
			materials[i].color.g = baseColor[1].GetFloat();
			materials[i].color.b = baseColor[2].GetFloat();
			materials[i].color.a = baseColor[3].GetFloat();
		}
		materials[i].metallic = value[i]["pbrMetallicRoughness"]["metallicFactor"].GetFloat();
	}

	Mesh* meshes = nullptr;
	value = json["meshes"]; 
	result->meshesCount = value.Size();
	meshes = new Mesh[result->meshesCount];
	result->meshes = meshes;
	for (GLuint i = 0; i < result->meshesCount; i++)
	{
		if (!value[i].HasMember("primitives") || !value[i]["primitives"].IsArray() || value[i]["primitives"].Empty())
		{
			std::cout << "LOADER::GLTF::MESHES::PRIMITIVES Message: Could not find meshes' primitives array." << std::endl;
			continue;
		}
		rapidjson::Value& primitives = value[i]["primitives"];
		meshes[i].primitivesCount = primitives.Size();
		meshes[i].primitives = new Primitive[meshes[i].primitivesCount];
		meshes[i].boundingBoxes = new Box[meshes[i].primitivesCount];
		for (GLuint j = 0; j < meshes[i].primitivesCount; j++)
		{
			if (!primitives[j].HasMember("attributes"))
			{
				std::cout << "LOADER::GLTF::MESHES::PRIMITIVES::ATTRIBUTES Message: Could not find meshes.primitives.attributtes." << std::endl;
				delete[] buffers;
				delete[] views;
				delete[] meshes;
				return result;
			}

			rapidjson::Value& attributes = primitives[j]["attributes"];
			//Get accesor for each attribute
			GLuint positions = attributes["POSITION"].GetUint();
			GLuint normals = attributes["NORMAL"].GetUint();
			GLuint tangents = attributes["TANGENT"].GetUint();
			GLuint texCoords0 = attributes["TEXCOORD_0"].GetUint();
			GLuint indicesAccess = primitives[j]["indices"].GetUint();

			GLuint material = primitives[j]["material"].GetUint();
			GLuint indicesCount = accessors[indicesAccess].count;
			GLuint verticesCount = accessors[positions].count;
			GLuint *indices = new GLuint[indicesCount];
			Vertex *vertices = new Vertex[verticesCount];

			for (GLuint k = 0; k < indicesCount; k++)
			{
				BufferView* view = &views[indicesAccess];
				Buffer* buffer = &buffers[view->buffer];
				switch (accessors[indicesAccess].componentType)
				{
				case GL_BYTE:
					indices[k] = *(GLbyte*)&buffer->data[view->offset + (accessors[indicesAccess].size * k)];
					break;
				case GL_UNSIGNED_BYTE:
					indices[k] = *(GLubyte*)&buffer->data[view->offset + (accessors[indicesAccess].size * k)];
					break;
				case GL_SHORT:
					indices[k] = *(GLshort*)&buffer->data[view->offset + (accessors[indicesAccess].size * k)];
					break;
				case GL_UNSIGNED_SHORT:
					indices[k] = *(GLushort*)&buffer->data[view->offset + (accessors[indicesAccess].size * k)];
					break;
				case GL_UNSIGNED_INT:
					indices[k] = *(GLuint*)&buffer->data[view->offset + (accessors[indicesAccess].size * k)];
					break;
				}
			}

			for (GLuint k = 0; k < verticesCount; k++)
			{
				BufferView* viewPos = &views[accessors[positions].view];
				BufferView* viewNor = &views[accessors[normals].view];
				BufferView* viewTan = &views[accessors[tangents].view];
				BufferView* viewTCo = &views[accessors[texCoords0].view];
				Buffer* bufferPos = &buffers[viewPos->buffer];
				Buffer* bufferNor = &buffers[viewNor->buffer];
				Buffer* bufferTan = &buffers[viewTan->buffer];
				Buffer* bufferTCo = &buffers[viewTCo->buffer];

				GLuint dataStride = this->GetComponentSize(accessors[positions].componentType);
				vertices[k].position.x = endian.littleFloat(*(GLfloat*)&bufferPos->data[viewPos->offset + (accessors[positions].size * k)]);
				vertices[k].position.y = endian.littleFloat(*(GLfloat*)&bufferPos->data[viewPos->offset + (accessors[positions].size * k) + dataStride]);
				vertices[k].position.z = endian.littleFloat(*(GLfloat*)&bufferPos->data[viewPos->offset + (accessors[positions].size * k) + (dataStride * 2)]);

				dataStride = this->GetComponentSize(accessors[normals].componentType);
				vertices[k].normal.x = endian.littleFloat(*(GLfloat*)&bufferNor->data[viewNor->offset + (accessors[normals].size * k)]);
				vertices[k].normal.y = endian.littleFloat(*(GLfloat*)&bufferNor->data[viewNor->offset + (accessors[normals].size * k) + dataStride]);
				vertices[k].normal.z = endian.littleFloat(*(GLfloat*)&bufferNor->data[viewNor->offset + (accessors[normals].size * k) + (dataStride * 2)]);

				dataStride = this->GetComponentSize(accessors[normals].componentType);
				vertices[k].tangent.x = endian.littleFloat(*(GLfloat*)&bufferTan->data[viewTan->offset + (accessors[tangents].size * k)]);
				vertices[k].tangent.y = endian.littleFloat(*(GLfloat*)&bufferTan->data[viewTan->offset + (accessors[tangents].size * k) + dataStride]);
				vertices[k].tangent.z = endian.littleFloat(*(GLfloat*)&bufferTan->data[viewTan->offset + (accessors[tangents].size * k) + (dataStride * 2)]);

				dataStride = this->GetComponentSize(accessors[texCoords0].componentType);
				vertices[k].texCoord0.x = endian.littleFloat(*(GLfloat*)&bufferPos->data[viewTan->offset + (accessors[texCoords0].size * k)]);
				vertices[k].texCoord0.y = endian.littleFloat(*(GLfloat*)&bufferPos->data[viewTan->offset + (accessors[texCoords0].size * k) + dataStride]);
			}

			Box *boundingBox = &meshes[i].boundingBoxes[j];
			boundingBox->bounds[1].x = ((GLfloat*)accessors[positions].max)[0];
			boundingBox->bounds[1].y = ((GLfloat*)accessors[positions].max)[1];
			boundingBox->bounds[1].z = ((GLfloat*)accessors[positions].max)[2];
			boundingBox->bounds[0].x = ((GLfloat*)accessors[positions].min)[0];
			boundingBox->bounds[0].y = ((GLfloat*)accessors[positions].min)[1];
			boundingBox->bounds[0].z = ((GLfloat*)accessors[positions].min)[2];
			meshes[i].primitives[j].setup(vertices, verticesCount, indices, indicesCount, material);
		}
	}


	Node* nodes;
	value = json["nodes"];
	result->nodesCount = value.Size();
	nodes = new Node[result->nodesCount];
	result->nodes = nodes;
	for (GLuint i = 0; i < result->nodesCount; i++)
	{
		Node* node = &nodes[i];
		Box boundingBox;
		boundingBox.bounds[1] = glm::vec3(-std::numeric_limits<GLfloat>::max());
		boundingBox.bounds[0] = glm::vec3(std::numeric_limits<GLfloat>::max());
		if (value[i].HasMember("mesh"))
		{
			node->mesh = value[i]["mesh"].GetUint();
			node->hasMesh = GL_TRUE;
			Mesh *mesh = &meshes[node->mesh];
			for (GLuint j = 0; j < mesh->primitivesCount; j++)
			{
				Box tmpBox = mesh->boundingBoxes[j];
				boundingBox.bounds[1].x = tmpBox.bounds[1].x > boundingBox.bounds[1].x ? tmpBox.bounds[1].x : boundingBox.bounds[1].x;
				boundingBox.bounds[1].y = tmpBox.bounds[1].y > boundingBox.bounds[1].y ? tmpBox.bounds[1].y : boundingBox.bounds[1].y;
				boundingBox.bounds[1].z = tmpBox.bounds[1].z > boundingBox.bounds[1].z ? tmpBox.bounds[1].z : boundingBox.bounds[1].z;

				boundingBox.bounds[0].x = tmpBox.bounds[0].x < boundingBox.bounds[0].x ? tmpBox.bounds[0].x : boundingBox.bounds[0].x;
				boundingBox.bounds[0].y = tmpBox.bounds[0].y < boundingBox.bounds[0].y ? tmpBox.bounds[0].y : boundingBox.bounds[0].y;
				boundingBox.bounds[0].z = tmpBox.bounds[0].z < boundingBox.bounds[0].z ? tmpBox.bounds[0].z : boundingBox.bounds[0].z;
			}
		}
		if (value[i].HasMember("children") && value[i]["children"].IsArray() && !value[i]["children"].Empty())
		{
			node->childrenCount = value[i]["children"].Size();
			node->children = new GLuint[node->childrenCount];
			for (GLuint j = 0; j < node->childrenCount; j++)
			{
				node->children[j] = value[i]["children"][j].GetUint();
				nodes[node->children[j]].parent = i;
				nodes[node->children[j]].isRoot = GL_FALSE;
			}
		}
		node->translation = glm::vec3(0.0);
		node->scale = glm::vec3(1.0);
		node->boundingBox = boundingBox;
		if (value[i].HasMember("translation") && value[i]["translation"].IsArray() && !value[i]["translation"].Empty())
		{
			node->translation.x = value[i]["translation"][0].GetFloat();
			node->translation.y = value[i]["translation"][1].GetFloat();
			node->translation.z = value[i]["translation"][2].GetFloat();
		}
		if (value[i].HasMember("scale") && value[i]["scale"].IsArray() && !value[i]["scale"].Empty())
		{
			node->scale.x = value[i]["scale"][0].GetFloat();
			node->scale.y = value[i]["scale"][1].GetFloat();
			node->scale.z = value[i]["scale"][2].GetFloat();
		}
	}

	Scene* scenes;
	value = json["scenes"];
	result->scenesCount = value.Size();
	scenes = new Scene[result->scenesCount];
	result->scenes = scenes;
	for (GLuint i = 0; i < result->scenesCount; i++)
	{
		Scene* scene = &scenes[i];
		rapidjson::Value& sceneNodes = value[i]["nodes"];
		scene->nodesCount = sceneNodes.Size();
		scene->nodes = new GLuint[scene->nodesCount];
		for (GLuint j = 0; j < scene->nodesCount; j++)
		{
			scene->nodes[j] = sceneNodes[j].GetUint();
		}
	}

	result->setup();

	delete[] buffers;
	delete[] views;
	delete[] accessors;
	return result;
}