#pragma once

#include "Geometry3D.h"
#include <vector>


typedef struct OctreeNode {
	AABB bounds;
	std::vector<OctreeNode> children;
	std::vector<Model*> models;
} OctreeNode;
class Scene {
protected:
	std::vector<Model*> objects;
	OctreeNode* octree;
private:
	Scene(const Scene&);
	Scene& operator=(const Scene&) {}
public:
	void AddModel(Model* model);
	void RemoveModel(Model* model);
	void UpdateModel(Model* model);
	std::vector<Model*> FindChildren(const Model* model);

	Model* Raycast(const Ray& ray);
	std::vector<Model*> Query(const Sphere& sphere);
	std::vector<Model*> Query(const AABB& aabb);

	bool Accelerate(const vec3& position, float size);

	inline Scene() : octree(nullptr) {}
	inline ~Scene() {
		if (octree != nullptr)
			delete octree;
	}
};

void SplitTree(OctreeNode* node, int depth);

void Insert(OctreeNode* node, Model* model);
void Remove(OctreeNode* node, Model* model);
void Update(OctreeNode* node, Model* model);

Model* FindClosest(const std::vector<Model*>& set, const Ray& ray);
Model* Raycast(OctreeNode* node, const Ray& ray);
std::vector<Model*> Query(OctreeNode* node, const Sphere& sphere);
std::vector<Model*> Query(OctreeNode* node, const AABB& aabb);