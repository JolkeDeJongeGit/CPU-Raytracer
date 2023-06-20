#pragma once
#include "BaseObject.h"
namespace rtc
{
	class Sphere : public BaseObject
	{
	public:
		Sphere(Util::point3 a_Center, float a_Radius, Material a_Mat) : m_Center(a_Center), m_Radius(a_Radius)
		{
			m_Mat = a_Mat;
			m_SqrRadius = m_Radius * m_Radius;
			m_InversRadius = 1.f / m_Radius;
		}
		bool HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData) override;
		void BoundingBox(Util::Box& a_Box) const override;
		void UV(const Util::point3& p, float& u, float& v);
		Util::point3 m_Center;
		float m_Radius;
		float m_SqrRadius;
		float m_InversRadius;
	private:
	};
}

