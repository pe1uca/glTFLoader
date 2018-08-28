#include "QuadTree.h"
#include <queue>

template<typename T>
int QuadTreeNode<T>::maxDepth = 5;
template<typename T>
int QuadTreeNode<T>::maxObjectPerNode = 15;

template<typename T>
bool QuadTreeNode<T>::IsLeaf()
{
	return children.size() == 0;
}

template<typename T>
int QuadTreeNode<T>::NumObjects()
{
	Reset();
	int objectCount = contents.size();
	for (auto it = contents.begin(); it != contents.end(); ++it)
	{
		(*it)->flag = true;
	}
	std::queue<QuadTreeNode<T>*> process;
	process.push(this);
	while (process.size() > 0)
	{
		QuadTreeNode<T>* processing = process.back();
		if (!processing->IsLeaf())
		{
			for (auto it = processing->children.begin(); it != processing->children.end(); ++it)
			{
				process.push(&(*it));
			}
		}
		else
		{
			for (auto it = processing->contents.begin(); it != processing->contents.end(); ++it)
			{
				if (!(*it)->flag)
				{
					++objectCount;
					(*it)->flag = true;
				}
			}
		}
		process.pop();
	}
	Reset();
	return objectCount;
}

template<typename T>
void QuadTreeNode<T>::Insert(QuadTreeData<T>& data)
{
	if (!RectangleRectangle(data.bounds, nodeBounds))
		return; // The object does not fit into this node
	if (IsLeaf() && contents.size() + 1 > maxObjectPerNode)
		Split();
	if (IsLeaf())
		contents.push_back(&data);
	else
		for (auto it = children.begin(); it != children.end(); ++it)
			(*it).Insert(data);
}

template<typename T>
void QuadTreeNode<T>::Remove(QuadTreeData<T>& data)
{
	if (IsLeaf())
	{
		int removeIndex = -1;
		for (auto it = contents.begin(); it != contents.end(); ++it)
		{
			if ((*it)->object == data.object)
			{
				contents.erase(it);
				break;
			}
		}
	}
	else
	{
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			(*it).Remove(data);
		}
	}
	Shake();
}

template<typename T>
void QuadTreeNode<T>::Update(QuadTreeData<T>& data)
{
	Remove(data);
	Insert(data);
}

template<typename T>
void QuadTreeNode<T>::Reset()
{
	if (IsLeaf())
	{
		for (auto it = contents.begin(); it != contents.end(); ++it)
			(*it)->flag = false;
	}
	else
	{
		for (auto it = children.begin(); it != children.end(); ++it)
			(*it).Reset();
	}
}

template<typename T>
void QuadTreeNode<T>::Shake()
{
	if (!IsLeaf())
	{
		int numObjects = NumObjects();
		if (numObjects == 0)
			children.clear();
		else if (numObjects < maxObjectPerNode)
		{
			std::queue<QuadTreeNode*> process;
			process.push(this);
			while (process.size() > 0)
			{
				QuadTreeNode* processing = process.back();
				if (!processing->IsLeaf())
				{
					for (auto it = processing->children.begin(); it != processing->children.end(); ++it)
						process.push(&(*it));
				}
				else
				{
					contents.insert(contents.end(), processing->contents.begin(), processing->contents.end());
				}
				process.pop();
			}
			children.clear();
		}
	}
}

template<typename T>
void QuadTreeNode<T>::Split()
{
	if (currentDepth + 1 >= maxDepth)
		return;
	vec2 min = GetMin(nodeBounds);
	vec2 max = GetMax(nodeBounds);
	vec2 center = min + ((max - min) * 0.5f);
	Rectangle2D childAreas[] = {
		Rectangle2D(
			FromMinMax(
				vec2(min.x, min.y),
				vec2(center.x, center.y))),
		Rectangle2D(
			FromMinMax(
				vec2(center.x, min.y),
				vec2(max.x, center.y))),
		Rectangle2D(
			FromMinMax(
				vec2(center.x, center.y),
				vec2(max.x, max.y))),
		Rectangle2D(
			FromMinMax(
				vec2(min.x, center.y),
				vec2(center.x, max.y)))
	};

	for (int i = 0; i < 4; ++i)
	{
		children.push_back(QuadTreeNode(childAreas[i]));
		children[i].currentDepth = currentDepth + 1;
	}
	for (int i = 0, size = contents.size(); i < size; ++i)
	{
		children[i].Insert(*contents[i]);
	}
	contents.clear();
}

template<typename T>
std::vector<QuadTreeData<T>*> QuadTreeNode<T>::Query(const Rectangle2D& area)
{
	std::vector<QuadTreeData<T>*> result;
	if (!RectangleRectangle(area, nodeBounds))
		return result;
	if (IsLeaf())
	{
		for (auto it = contents.begin(); it != contents.end(); ++it)
		{
			if (RectangleRectangle((*it)->bounds, area))
			{
				result.push_back((*it));
			}
		}
	}
	else
	{
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			auto recurse = (*it).Query(area);
			if (recurse.size() > 0)
				result.insert(result.end(), recurse.begin(), recurse.end());
		}
	}
	return result;
}