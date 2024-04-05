#pragma once
#include <stdio.h>
#include <math.h>
#include <map>
#include <string>

#include <ew/external/glad.h>
#include <ew/transform.h>

namespace wr
{

	struct TransformNode
	{
		ew::Transform myLocalTransform;
		ew::Transform myGlobalTransform;
		TransformNode* myParent;
		bool isRoot = false;
		bool updated = false;
	};

	class Hierarchy
	{
	public:
		Hierarchy(ew::Transform rootTrans);
		void addTransform(ew::Transform localTransform, std::string transformName, std::string parentName);
		void setLocalTransform(std::string transformName, ew::Transform localTransform);
		ew::Transform getWorldTransform(std::string transformName);
		ew::Transform getLocalTransform(std::string transformName);
		void updateTransforms();

	private:
		std::map<std::string, TransformNode> hMap;

		void calcWorldTransform(TransformNode* node);
		void resetUpdated();
	};

}