#include "precomp.h"
#include "Sphere.h"
namespace rtc
{
	
	bool Sphere::HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData)
	{
		Util::vec3 directionSphere = a_R.m_Orig - m_Center; // gives the distance from the sphere to the ray orgin
		float a = a_R.m_Dir.sqrLength();
		float b = directionSphere.dot(a_R.m_Dir); // The projection length of the ray vector 
		float c = directionSphere.sqrLength() - m_SqrRadius; // It gets the distance from the ray to the shell.
		float discriminant = b * b -  a * c; // to calculate the discriminant b*b - 4ac
		if (discriminant < 0) return false; // when the discriminant is smaller than 0 then we know it doesn't hit an sphere
		float sqrtfd = Util::betterSqrt(discriminant);
		float inverseA = 1.f / a;

		float t = (-b - sqrtfd) * inverseA; //We will calculate the t value using the quadratic formula

		// Find the nearest root that lies in the acceptable range.
		if (t < a_Min || t > a_Max)
		{
			t = (-b + sqrtfd) * inverseA;
			if (t < a_Min || t > a_Max)
				return false;
		}

		a_HitData.t = t; // Tells the dataHit what the length is of vector where it has hit the shell
		a_HitData.pointIntersection = a_R.At(t); // Return the point where it has intersected
		Util::vec3 outwardNormal = (a_HitData.pointIntersection - m_Center) * m_InversRadius;
		a_HitData.SetFace(a_R, outwardNormal); // normilize the way where it is pointing at
		//UV(outwardNormal, a_HitData.u, a_HitData.v);
		a_HitData.material = m_Mat;

		return true;
	}
	void Sphere::BoundingBox(Util::Box& a_Box) const
	{
		a_Box.min = m_Center - Util::vec3(m_Radius);
		a_Box.max = m_Center + Util::vec3(m_Radius);
	}

	void Sphere::UV(const Util::point3& p, float& u, float& v)
	{
		float theta = acosf(-p.y);
		float phi = atan2f(-p.z, p.x) + Util::PI;

		u = phi * Util::INVERSE_PI_2;
		v = theta * Util::INVERSE_PI;
	}
}
