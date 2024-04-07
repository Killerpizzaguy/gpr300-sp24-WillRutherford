#pragma once
#include <stdio.h>
#include <math.h>
#include <map>
#include <string>
#include <vector>

#include "../ew/external/glad.h"
#include "../ew/transform.h"

namespace wr
{

	struct TransformNode
	{
		ew::Transform myLocalTransform;
		glm::mat4 myGlobalTransform;
		TransformNode* myParent;
		bool isRoot = false;
		bool updated = false;
		int myDepth;
	};

	class Hierarchy
	{
	public:
		Hierarchy();
		Hierarchy(ew::Transform rootTrans);
		void addTransform(ew::Transform localTransform, std::string transformName, std::string parentName);
		void addTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, std::string transformName, std::string parentName);
		void setLocalTransform(std::string transformName, ew::Transform localTransform);
		glm::mat4 getWorldTransform(std::string transformName);
		ew::Transform getLocalTransform(std::string transformName);
		void updateTransforms();
		void resetUpdated();

		std::map<std::string, TransformNode> hMap;

	private:
		int depthTracker = 0;
		std::vector <std::string> depthToStringList;
		void calcWorldTransform(TransformNode* node);
		glm::mat4 multTransforms(glm::mat4 parent, ew::Transform child);
	};

}