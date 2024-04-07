#include "hierarchy.h"
#include <iostream>

namespace wr 
{

	Hierarchy::Hierarchy()
	{
		TransformNode root;
		root.isRoot = true;
		ew::Transform rootTrans;

		root.myLocalTransform = rootTrans;
		root.myGlobalTransform = rootTrans.modelMatrix();
		depthTracker = 0;
		root.myDepth = depthTracker;
		depthToStringList.push_back("root");
		depthTracker++;
		hMap["root"] = root;
	}

	Hierarchy::Hierarchy(ew::Transform rootTrans)
	{
		TransformNode root;
		root.isRoot = true;
		root.myLocalTransform = rootTrans;
		root.myGlobalTransform = rootTrans.modelMatrix();
		depthTracker = 0;
		root.myDepth = depthTracker;
		depthToStringList.push_back("root");
		depthTracker++;
		hMap["root"] = root;
	}

	void Hierarchy::addTransform(ew::Transform localTransform, std::string transformName, std::string parentName)
	{
		TransformNode node;
		node.myLocalTransform = localTransform;
		node.myParent = & hMap[parentName];
		node.isRoot = false;
		node.myDepth = depthTracker;
		depthToStringList.push_back(transformName);
		depthTracker++;
		hMap[transformName] = node;
	}

	void Hierarchy::addTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, std::string transformName, std::string parentName)
	{
		TransformNode node;
		node.myLocalTransform.position = pos;
		node.myLocalTransform.scale = scl;
		rot = glm::vec3(glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z));
		node.myLocalTransform.rotation = glm::quat(rot);
		
		std::cout << "added node " << transformName << " with depth " << depthTracker << std::endl;
		node.myDepth = depthTracker;
		depthToStringList.push_back(transformName);
		depthTracker++;
		node.myParent = &hMap[parentName];
		hMap[transformName] = node;
	}

	void Hierarchy::setLocalTransform(std::string transformName, ew::Transform localTransform)
	{
		hMap[transformName].myLocalTransform = localTransform;
	}

	glm::mat4 Hierarchy::getWorldTransform(std::string transformName)
	{
		return hMap[transformName].myGlobalTransform;
	}

	ew::Transform Hierarchy::getLocalTransform(std::string transformName)
	{
		return hMap[transformName].myLocalTransform;
	}

	void Hierarchy::updateTransforms()
	{
		/*std::map<std::string, TransformNode>::iterator it = hMap.begin();
		
		while (it != hMap.end())
		{
			calcWorldTransform(& it->second);
			it++;
		}*/

		for each (std::string name in depthToStringList)
		{
			calcWorldTransform(&hMap[name]);
		}
	}

	void Hierarchy::calcWorldTransform(TransformNode* node)
	{
		if (node->isRoot)
		{
			node->myGlobalTransform = node->myLocalTransform.modelMatrix();
			return;
		}

		else if (node->updated)
		{
			return;
		}

		else 
		{
			if (!node->myParent->updated)
			{
				calcWorldTransform(node->myParent);
			}
			
			node->myGlobalTransform = node->myParent->myGlobalTransform * node->myLocalTransform.modelMatrix();
			//node->myGlobalTransform = multTransforms(node->myParent->myGlobalTransform, node->myLocalTransform);
			node->updated = true;
		}
	}

	glm::mat4 Hierarchy::multTransforms(glm::mat4 parent, ew::Transform child)
	{
		return parent * child.modelMatrix();
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