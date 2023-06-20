#pragma once
#include <xmmintrin.h>
namespace Util
{
	//struct Box
	//{
	//	Box() {}
	//	Box(point3 const& a_Min, point3 const& a_Max) : min(a_Min), max(a_Max) 
	//	{
	//		center = min + min * .5f;
	//	}
	//	point3 min = (0.f);
	//	point3 max = (0.f);
	//	point3 center = (0.f);
	//	inline point3 GetDistance() const { return max - min; };
	//	inline float Area() {
	//		Util::point3 dist = GetDistance();
	//		return (dist.x * dist.y + dist.x * dist.z + dist.z * dist.y) * 2;
	//	};
	//	inline Box& Expand(Box const& a_Box) {
	//		min = Min(min, a_Box.min);
	//		max = Max(max, a_Box.max);
	//		center = min + min * .5f;
	//		return *this;
	//	};
	//};

	struct Box
	{
		Box() {}
		union
		{
			point3 min = (0.f);
			__m128 m_MinM;
		};
		union
		{
			point3 max = (0.f);
			__m128 m_MaxM;
		};
	};
	inline point3 GetDistance(Box const& a) { return a.max - a.min; };
	inline float Area(Box const& a) {
		Util::point3 dist = GetDistance(a);
		return (dist.x * dist.y + dist.x * dist.z + dist.z * dist.y) * 2;
	};

	inline point3 Center(Box const& a) 
	{
		return a.min + a.min * .5f;
	};
	inline Box Expand(Box const& a_BoxA, Box const& a_BoxB ) 
	{
		Box c;
		c.min = Min(a_BoxA.min, a_BoxB.min);
		c.max = Max(a_BoxA.max, a_BoxB.max);

		return c;
	};
}