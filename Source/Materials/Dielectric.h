#pragma once
#include "BaseObject.h"
namespace rtc
{
	class Dielectric
	{
	public:
        Dielectric(Util::color a_Color, float a_IndexReflection) : m_Albedo(a_Color), m_IndexReflection(a_IndexReflection), m_IndexAir(1.f/ a_IndexReflection) {}

		void Scatter(const Util::Ray& a_Ray, const HitData& a_HitData, Util::color& attenuation, Util::Ray& scattered) const
		{
            attenuation = m_Albedo;
            float refractionInside = m_IndexReflection;
            float refractionOutside = 1.f;
            float refractionIndex = m_IndexAir;

            if (!a_HitData.frontFace)
            {
                std::swap(refractionInside, refractionOutside);
                float distance = (a_HitData.pointIntersection).sqrLength();
                attenuation *= expf(-(.003f * distance));
                refractionIndex = m_IndexReflection;
            }

            Util::vec3 Direction = a_Ray.Direction().normalized();
            float cosThetaO = fminf(-(Direction).dot(a_HitData.normal), 1.0f); // Gets the angle between the norm and the direction of the ray
            float sinThetaO = 1.f - cosThetaO * cosThetaO;

            bool cannotRefract = refractionIndex * sinThetaO > 1.0f;
            Util::vec3 newDirection;

            if (cannotRefract || reflectance(cosThetaO, refractionInside, refractionOutside) > Util::frand())
                newDirection = reflect(Direction, a_HitData.normal);
            else
                newDirection = refract(Direction, a_HitData.normal, refractionIndex);

            scattered = Util::Ray(a_HitData.pointIntersection, newDirection);
		};
    private:
        Util::color m_Albedo;
        float m_IndexReflection = 1.f;
        float m_IndexAir = 1.f;
        float reflectance(float cosoutside, float ninside, float noutside) const
        {
            //Fernels equations
            float sint = noutside / ninside * sqrtf(std::max(0.f, 1 - cosoutside * cosoutside));
            if (sint >= 1)
                return 1.f;

            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosoutside = fabsf(cosoutside);
            float Rs = ((ninside * cosoutside) - (noutside * cost)) / ((ninside * cosoutside) + (noutside * cost));
            float Rp = ((ninside * cosoutside) - (ninside * cost)) / ((noutside * cosoutside) + (ninside * cost));
            return (Rs * Rs + Rp * Rp) * .5f;
        }
	};
}