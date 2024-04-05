#include "hierarchy.h"

namespace wr 
{

	Hierarchy::Hierarchy(ew::Transform rootTrans)
	{
		TransformNode root;
		root.isRoot = true;
		root.myLocalTransform = rootTrans;
		root.myGlobalTransform = rootTrans;
		hMap["root"] = root;
	}

	void Hierarchy::addTransform(ew::Transform localTransform, std::string transformName, std::string parentName)
	{
		TransformNode node;
		node.myLocalTransform = localTransform;
		node.myParent = hMap[parentName];
		hMap[transformName] = node;
	}

	void Hierarchy::setLocalTransform(std::string transformName, ew::Transform localTransform)
	{
		hMap[transformName].myLocalTransform = localTransform;
	}

	ew::Transform Hierarchy::getWorldTransform(std::string transformName)
	{
		return hMap[transformName].myGlobalTransform;
	}

	ew::Transform Hierarchy::getLocalTransform(std::string transformName)
	{
		return hMap[transformName].myLocalTransform;
	}

	void Hierarchy::updateTransforms()
	{
		std::map<std::string, TransformNode>::iterator it = hMap.begin();
		
		while (it != hMap.end())
		{
			calcWorldTransform(it->second);
			it++;
		}
	}

	void Hierarchy::calcWorldTransform(TransformNode* node)
	{
		if (node->isRoot)
		{
			node->myGlobalTransform = node->myLocalTransform;
			return;
		}

		else if (node->updated)
		{
			return;
		}

		else 
		{
			calcWorldTransform(node->myParent);
			node->myGlobalTransform = node->myParent->myGlobalTransform * node->myLocalTransform;
			node->updated = true;
		}
	}

	void Hierarchy::resetUpdated()
	{
		std::map<std::string, TransformNode>::iterator it = hMap.begin();

		while (it != hMap.end())
		{
			it->second.updated = false;
			it++;
		}
	}
}