#pragma once
#include "BaseObject.h"
namespace rtc
{
	class Metal
	{
	public:
		Metal(Util::color a_Albedo, float a_Fuzz) : m_Albedo(a_Albedo), m_Fuzz(a_Fuzz < 1 ? a_Fuzz : 1) {};
		bool Scatter(const Util::Ray& a_Ray, const HitData& a_HitData, Util::color& attenuation, Util::Ray& scattered) const
		{
			Util::vec3 reflection = reflect(a_Ray.Direction(), a_HitData.normal); // Gets the reflection based on where the the direction hit
			attenuation = m_Albedo;
			if (m_Fuzz > 0.f)
			{
				scattered = Util::Ray(a_HitData.pointIntersection, reflection + m_Fuzz * Util::RandomUnitSphere()); // Here I create a new ray with a slight offset based on the fuzzines
				return scattered.Direction().dot(a_HitData.normal) > 0; // So that it won't reflect inwards
			}

			scattered = Util::Ray(a_HitData.pointIntersection, reflection); // Here I create a new ray with a slight offset based on the fuzzines
			return scattered.Direction().dot(a_HitData.normal) > 0; // So that it won't reflect inwards
		};
		Util::color m_Albedo;
		float m_Fuzz = 0;
	};
};