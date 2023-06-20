#pragma once
#include "BaseObject.h"
#include "Raytracer/BVHNode.h"
#include "Camera.h"
namespace rtc
{
	class PointLight;
	class Scene
	{
	public:
		void LoadScene(int a_SceneId);
		bool Hit(const Util::Ray& r, float t_min, float t_max, HitData& rec);
		std::vector<BaseObject*> m_SceneList;
		std::vector<std::unique_ptr<PointLight>> m_LightList;
		Util::BVHNode<BaseObject*> m_TreeNode;
		Camera m_Camera;
		int m_SceneID = 0;
	};
}

