#pragma once
namespace rtc
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(Util::vec3 a_Pos, Util::color a_C, float a_Intensity) : m_Pos(a_Pos), m_C(a_C), m_Intensity(a_Intensity) {}
		Util::vec3 m_Pos;
		Util::color m_C;
		float m_Intensity = 10;
	};
}

