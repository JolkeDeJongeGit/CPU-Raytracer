#pragma once
namespace rtc
{
	class Texture
	{
	public:
		Util::color CheckerTexture(Util::point3 const& a_P, Util::color odd, Util::color even)
		{
			auto sines = sinf(10 * a_P.x) * sin(10 * a_P.y) * sin(10 * a_P.z);
			if (sines < 0)
				return odd;
			else
				return even;
		}
	};
}

