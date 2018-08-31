#include "Scene.h"
#include <algorithm>
#include <stack>

void Scene::AddModel(Model* model)
{
	if (std::find(objects.begin(), objects.end(), model) != objects.end())
		return;

	objects.push_back(model);
}
void Scene::RemoveModel(Model* model)
{
	objects.erase(std::remove(objects.begin(), objects.end(), model), objects.end());
}
void Scene::UpdateModel(Model* /*model*/)
{
	//Placeholder
}
std::vector<Model*> Scene::FindChildren(const Model* model)
{
	std::vector<Model*> result;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it) == nullptr || (*it) == model)
			continue;

		Model* iterator = (*it)->parent;
		if (iterator != nullptr)
		{
			if (iterator == model)
			{
				result.push_back((*it));
				continue;
			}
			iterator = iterator->parent;
		}
	}
	return result;
}
Model* Scene::Raycast(const Ray& ray)
{
	if (octree != nullptr)
		return ::Raycast(octree, ray);
	Model* result = nullptr;
	float result_t = -1;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		float t = ModelRay(*(*it), ray);
		if (result == nullptr && t >= 0.0f)
		{
			result = (*it);
			result_t = t;
		}
		else if(result != nullptr && t < result_t)
		{
			result = (*it);
			result_t = t;
		}
	}

	return result;
}
std::vector<Model*> Scene::Query(const Sphere& sphere)
{
	if (octree != nullptr)
		return ::Query(octree, sphere);
	std::vector<Model*> result;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (SphereOBB(sphere, GetOBB(*(*it))))
			result.push_back((*it));
	}
	return result;
}
std::vector<Model*> Scene::Query(const AABB& aabb)
{
	if (octree != nullptr)
		return ::Query(octree, aabb);
	std::vector<Model*> result;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (AABBOBB(aabb, GetOBB(*(*it))))
			result.push_back((*it));
	}
	return result;
}
bool Scene::Accelerate(const vec3& position, float size)
{
	if (octree != nullptr)
		return false;

	vec3 min(position.x - size, position.y - size, position.z - size);
	vec3 max(position.x + size, position.y + size, position.z + size);

	octree = new OctreeNode();
	octree->bounds = FromMinMax(min, max);
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		octree->models.push_back((*it));
	}

	SplitTree(octree, 5);
	return true;
}

void SplitTree(OctreeNode* node, int depth)
{
	if (depth-- <= 0)
		return;

	if (node->children.empty())
	{
		node->children.resize(8);
		vec3 c = node->bounds.position;
		vec3 e = node->bounds.size * 0.5f;

		node->children[0].bounds = AABB(c + vec3(-e.x, e.y, -e.z), e);
		node->children[1].bounds = AABB(c + vec3(e.x, e.y, -e.z), e);
		node->children[2].bounds = AABB(c + vec3(-e.x, e.y, e.z), e);
		node->children[3].bounds = AABB(c + vec3(e.x, e.y, e.z), e);
		node->children[4].bounds = AABB(c + vec3(-e.x, -e.y, -e.z), e);
		node->children[5].bounds = AABB(c + vec3(e.x, -e.y, -e.z), e);
		node->children[6].bounds = AABB(c + vec3(-e.x, -e.y, e.z), e);
		node->children[7].bounds = AABB(c + vec3(e.x, -e.y, e.z), e);
	}

	if (!node->children.empty() && !node->models.empty())
	{
		for (auto it = node->children.begin(); it != node->children.end(); ++it)
		{
			for (auto jt = node->models.begin(); jt != node->models.end(); ++jt)
			{
				if (AABBOBB((*it).bounds, GetOBB(*(*jt))))
					(*it).models.push_back((*jt));
			}
		}
		node->models.clear();
		node->models.shrink_to_fit();
		for (auto it = node->children.begin(); it != node->children.end(); ++it)
			SplitTree(&(*it), depth);
	}
}

void Insert(OctreeNode* node, Model* model)
{
	if (AABBOBB(node->bounds, GetOBB(*model)))
	{
		if (node->children.empty())
			node->models.push_back(model);
		else
			for (auto it = node->children.begin(); it != node->children.end(); ++it)
				Insert(&(*it), model);
	}
}
void Remove(OctreeNode* node, Model* model)
{
	if (node->children.empty())
	{
		auto it = std::find(node->models.begin(), node->models.end(), model);
		if (it != node->models.end())
			node->models.erase(it);
	}
	else
		for (auto it = node->children.begin(); it != node->children.end(); ++it)
			Remove(&(*it), model);
}
void Update(OctreeNode* node, Model* model)
{
	Remove(node, model);
	Insert(node, model);
}

Model* FindClosest(const std::vector<Model*>& set, const Ray& ray)
{
	if (set.empty())
		return nullptr;

	Model* closest = nullptr;
	float closest_t = -1;

	for (auto it = set.begin(); it != set.end(); ++it)
	{
		float this_t = ModelRay(*(*it), ray);
		if (this_t < 0.0f)
			continue;

		if (closest_t < 0.0f || this_t < closest_t)
		{
			closest_t = this_t;
			closest = (*it);
		}
	}

	return closest;
}
Model* Raycast(OctreeNode* node, const Ray& ray)
{
	float t = Raycast(node->bounds, ray);
	if (t >= 0.0f)
	{
		if (node->children.empty())
			return FindClosest(node->models, ray);
		else
		{
			std::vector<Model*> results;
			for (auto it = node->children.begin(); it != node->children.end(); ++it)
			{
				Model* result = Raycast(&(*it), ray);
				if (result != nullptr)
					results.push_back(result);
			}

			return FindClosest(results, ray);
		}
	}
	return nullptr;
}
std::vector<Model*> Query(OctreeNode* node, const Sphere& sphere)
{
	std::vector<Model*> result;
	if (SphereAABB(sphere, node->bounds))
	{
		if (node->children.empty())
		{
			for (auto it = node->models.begin(); it != node->models.end(); ++it)
			{
				if (SphereOBB(sphere, GetOBB(*(*it))))
					result.push_back((*it));
			}
		}
		else
		{
			for (auto it = node->children.begin(); it != node->children.end(); ++it)
			{
				std::vector<Model*> child = Query(&(*it), sphere);
				if (!child.empty())
					result.insert(result.end(), child.begin(), child.end());
			}
		}
	}

	return result;
}
std::vector<Model*> Query(OctreeNode* node, const AABB& aabb)
{
	std::vector<Model*> result;
	if (AABBAABB(aabb, node->bounds))
	{
		if (node->children.empty())
		{
			for (auto it = node->models.begin(); it != node->models.end(); ++it)
			{
				if (AABBOBB(aabb, GetOBB(*(*it))))
					result.push_back((*it));
			}
		}
		else
		{
			for (auto it = node->children.begin(); it != node->children.end(); ++it)
			{
				std::vector<Model*> child = Query(&(*it), aabb);
				if (!child.empty())
					result.insert(result.end(), child.begin(), child.end());
			}
		}
	}

	return result;
}
