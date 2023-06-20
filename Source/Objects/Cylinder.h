#pragma once
#include "BaseObject.h"
namespace rtc
{
	class Cylinder : public BaseObject
	{
	public:
		Cylinder(Util::point3 a_Center, float a_Radius, float a_Height, Material a_Mat) : m_Center(a_Center), m_Radius(a_Radius), m_Height(a_Height)
		{
			m_Mat = a_Mat;
		}
		bool HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData) override;
		void BoundingBox(Util::Box& a_Box) const override;
	private:
		Util::point3 m_Center;
		float m_Radius;
		float m_Height;
	};
}

