#pragma once
#include "Raytracer/Ray.h"
#include "Raytracer/Box.h"
namespace rtc
{
	inline Util::vec3 reflect(const Util::vec3& a_Direction, const Util::vec3& a_Normal) 
	{
		return a_Direction - 2 * a_Direction.dot(a_Normal) * a_Normal;
	}

	inline Util::vec3 refract(const Util::vec3& a_Direction, const Util::vec3& a_Normal, float a_RefractionIndex)
	{
		float cosInside = Util::Clamp(-1.f, 1.f, a_Direction.dot(a_Normal));
		Util::vec3 n = a_Normal;
		if (cosInside < 0) { cosInside = -cosInside; }
		else { n = -a_Normal; }
		float k = 1 - a_RefractionIndex * a_RefractionIndex * (1 - cosInside * cosInside);
		Util::vec3 t = a_Direction * a_RefractionIndex + (a_RefractionIndex * cosInside - Util::betterSqrt(k)) * a_Normal;
		return k < 0.f ? 0.f : t;
	}

	enum class MaterialType { LAMBERTIAN, METAL, DIELECTRIC };
	enum class TextureType { CHECKER, SOLID };

	struct Material
	{
		Material() = default;
		Material(Util::color a_Albedo, MaterialType a_Type, float a_CustomValue = 0, TextureType a_TType = TextureType::SOLID) : albedo(a_Albedo), type(a_Type), customValue(a_CustomValue), Ttype(a_TType){}
		Util::color albedo;
		MaterialType type = MaterialType::LAMBERTIAN;
		TextureType Ttype = TextureType::SOLID;
		float customValue = 0; // This is to either define fuzz or index refelection
	};
	/// <summary>
	/// Stores the Hit data 
	/// </summary>
	struct HitData
	{
		Util::point3 pointIntersection; // The coordinate on where it hits
		Util::vec3 normal; // Direction the normal is pointing at
		float t = 0; // Length based on the ray vector where it hit
		bool frontFace = false; // Tells if the ray hit the outward of surface or inwards
		Material material;
		float depth = 0;
		//float u = 0;
		//float v = 0; // Here we define the normilized coords on the object

		inline void SetFace(Util::Ray const& a_R, Util::vec3 const& a_OutwardNormal) {
			// If the ray direction is going the opposite side of the  a_OutwardNormal 
			// then we know that the ray hits the outside of the sphere
			frontFace = a_R.Direction().dot(a_OutwardNormal) < 0;
			normal = frontFace ? a_OutwardNormal : -a_OutwardNormal;
		}
	};

	/// <summary>
	/// Here I have an interface for every object I will create
	/// </summary>
	class BaseObject
	{
	public:
		virtual bool HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData) = 0;
		virtual void BoundingBox(Util::Box& a_Box) const = 0;
		Material m_Mat;
	};
}