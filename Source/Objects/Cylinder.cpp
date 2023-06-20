#include "precomp.h"
#include "Cylinder.h"
namespace rtc
{
	bool Cylinder::HitObject(const Util::Ray& a_R, float a_Min, float a_Max, HitData& a_HitData)
	{
		Util::vec3 directionSphere = a_R.Origin() - m_Center; // gives the distance from the sphere to the ray orgin
		float a = (a_R.Direction().x * a_R.Direction().x + a_R.Direction().z * a_R.Direction().z);
		float b = directionSphere.x * a_R.Direction().x + directionSphere.z * a_R.Direction().z; // The projection length of the ray vector 
		float c = (directionSphere.x * directionSphere.x + directionSphere.z * directionSphere.z) - m_Radius * m_Radius; // It gets the distance from the ray to the shell.

		Util::vec3 point1 = Util::vec3(m_Center.x, m_Center.y - (m_Height / 2), m_Center.z); // bottom cap
		Util::vec3 point2 = Util::vec3(m_Center.x, m_Center.y + (m_Height / 2), m_Center.z); // top cap

		float discriminant =b * b -  a*c; // to calculate the discriminant b*b - 4ac

		// when the discriminant is smaller than 0 then we know it doesn't hit an sphere
		if (discriminant > 0)
		{
			float sqrtfd = sqrtf(discriminant);
			float t = (-b - sqrtfd) / a; //We will calculate the t value using the quadratic formula
			// Find the nearest root that lies in the acceptable range.
			if (t < a_Min || t > a_Max)
			{
				t = (-b + sqrtfd) / a;
				if (t < a_Min || t > a_Max)
					return false;
			}

			a_HitData.t = t; // Tells the dataHit what the length is of vector where it has hit the shell
			a_HitData.pointIntersection = a_R.At(t); // Return the point where it has intersected
			Util::vec3 normal = Util::vec3(a_HitData.pointIntersection.x, 0.f, a_HitData.pointIntersection.z);
			//a_HitData.SetFace(a_R, (normal - m_Center).normalized()); // normilize the way where it is pointing at
			a_HitData.SetFace(a_R, normal - m_Center); // normilize the way where it is pointing at
			a_HitData.material = m_Mat;

			if (a_HitData.pointIntersection.y < point1.y || a_HitData.pointIntersection.y > point2.y)
				return false;

			return true;
		}
		return false;
	}
	void Cylinder::BoundingBox(Util::Box& a_Box) const
	{
		a_Box.min = m_Center - Util::vec3(m_Radius, m_Height / 2, m_Radius);
		a_Box.max = m_Center + Util::vec3(m_Radius, m_Height / 2, m_Radius);
	}
};