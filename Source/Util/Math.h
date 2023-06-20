#pragma once
namespace Util
{
	constexpr float PI = 3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f;
	constexpr float DegToRad = 1.f / 180.f;
	constexpr float RadToDeg = 1.f / PI;
	template <typename T>
	constexpr T Min(T const a, T const b) { return (a > b) ? b : a; }
	template <typename T>
	constexpr T Max(T const a, T const b) { return (a > b) ? a : b; }
	template <typename T>
	constexpr T Clamp(T const value, T const min, T const max) { return Min(max, Max(value, min)); }
	template <typename T>
	constexpr T Normilize(T const value, T const min, T const max) { return Clamp(0, 1, (value - min) / (max - min)); }
	template <typename T>
	constexpr T Lerp(T const value, T const min, T const max) { return (min + (max - min) * value); }
	template <typename T>
	constexpr float DegreeToRads(T const value) { return (value * PI * DegToRad); }
	template <typename T>
	constexpr float RadsToDegree(T const value) { return (value * 180.f * RadToDeg); }
}