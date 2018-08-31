#pragma once

#include "Geometry2D.h"
#include <vector>

template<typename T>
struct QuadTreeData {
	T* object;
	Rectangle2D bounds;
	bool flag;
	
	inline QuadTreeData(T* o, const Rectangle2D& b) :
		object(o), bounds(b), flag(false) {}
};

template<typename T>
class QuadTreeNode {
protected:
	std::vector<QuadTreeNode<T>> children;
	std::vector<QuadTreeData<T>*> contents;
	int currentDepth;
	static int maxDepth;
	static int maxObjectPerNode;
	Rectangle2D nodeBounds;

public:
	inline QuadTreeNode(const Rectangle2D& bounds) :
		nodeBounds(bounds), currentDepth(0) {}

	bool IsLeaf();
	int NumObjects();
	void Insert(QuadTreeData<T>& data);
	void Remove(QuadTreeData<T>& data);
	void Update(QuadTreeData<T>& data);
	void Shake();
	void Split();
	void Reset();
	std::vector<QuadTreeData<T>*> Query(const Rectangle2D& area);
};

template<typename T>
using QuadTree = QuadTreeNode<T>;
