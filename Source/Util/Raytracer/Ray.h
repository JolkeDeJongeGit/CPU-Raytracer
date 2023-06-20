#pragma once
namespace Util
{
	class Ray
	{
	public:
		Ray() {};
		Ray(Util::point3 a_Orig, Util::vec3 a_Dir) : m_Orig(a_Orig), m_Dir(a_Dir), m_RDir( 1.f / a_Dir )  {};
		Util::vec3 Direction() const { return m_Dir; }
		Util::point3 Origin() const { return m_Orig; }
		// return the position on based on the t value
		inline Util::point3 At(float t) const { return m_Orig + t * m_Dir; };
		Util::point3 m_Orig;
		Util::vec3 m_Dir; 
		Util::vec3 m_RDir; 
	private:
	};
}

