#pragma once
#include "BaseObject.h"
namespace rtc
{
	class Ellipsoid : public BaseObject
	{
	public:
		Ellipsoid(Util::point3 a_Center,  Util::vec3 a_Radius, Material a_Mat) : m_Center(a_Center), m_Radius(a_Radius) 
		{
			m_Mat = a_Mat;
		}
		bool HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData) override;
		void BoundingBox(Util::Box& a_Box) const override;
		void UV(const Util::point3& p, float& u, float& v);
	private:
		Util::point3 m_Center;
		Util::vec3 m_Radius;
		Util::vec3 m_inverseRadius = Util::vec3(1.f) / m_Radius;
		Util::vec3 m_inverseSqrRadius = Util::vec3(1.f) / (m_Radius * m_Radius);
	};
}