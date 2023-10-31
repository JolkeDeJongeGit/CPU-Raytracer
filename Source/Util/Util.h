#pragma once
#include <random>
#include <emmintrin.h>
namespace Util
{
	typedef unsigned char uchar;
	typedef unsigned char byte;
	typedef long long int64;
	typedef unsigned long long uint64;
	typedef unsigned int uint;
	typedef unsigned int Pixel;
	// identical to std::sqrt
	inline float betterSqrt(const float f)
	{
		__m128 temp = _mm_set_ss(f);
		temp = _mm_sqrt_ss(temp);
		return _mm_cvtss_f32(temp);
	}

	// faster than  1.0f/std::sqrt, but with little accuracy.
	inline float betterRsqrt(const float f)
	{
		__m128 temp = _mm_set_ss(f);
		temp = _mm_rsqrt_ss(temp);
		return _mm_cvtss_f32(temp);
	}

	// identical to std::sqrt
	inline double betterSqrt(const double f)
	{
		__m128d temp = _mm_set_sd(f);
		temp = _mm_sqrt_sd(temp, temp);
		return _mm_cvtsd_f64(temp);
	}

	// identical to 1.0 / std::sqrt
	// .... there isn't an instruction for rsqrt with double, 
	// so 1.0 / std::sqrt is the best you've got. 
	inline double betterRsqrt(const double f)
	{
		__m128d temp = _mm_set_sd(f);
		temp = _mm_div_sd(_mm_set_sd(1.0), _mm_sqrt_sd(temp, temp));
		return _mm_cvtsd_f64(temp);
	}

	class color
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { float r, g, b; }; float cell[3]; };
#pragma warning (pop)
		color() : r(0), g(0), b(0) {};
		color(float v) : r(v), g(v), b(v) {}
		color(float r, float g, float b) : r(r), g(g), b(b) { }
		color operator + (const color& addOperand) const { return color(r + addOperand.r, g + addOperand.g, b + addOperand.b); }
		color operator - (const color& operand) const { return color(r - operand.r, g - operand.g, b - operand.b); }
		color operator * (const color& operand) const { return color(r * operand.r, g * operand.g, b * operand.b); }
		color operator / (const color& operand) const { return color(r / operand.r, g / operand.g, b / operand.b); }
		void operator -= (const color& a) { r -= a.r; g -= a.g; b -= a.b; }
		void operator += (const color& a) { r += a.r; g += a.g; b += a.b; }
		void operator *= (const color& a) { r *= a.r; g *= a.g; b *= a.b; }
		void operator *= (const float a) { r *= a; g *= a; b *= a; }
		float operator [] (const uint& idx) const { return cell[idx]; }
		float& operator [] (const uint& idx) { return cell[idx]; }
		inline uint Get() { return (uint)(((int)(255 * Min(r, 1.0f)) << 16) + ((int)(255 * Min(g, 1.f)) << 8) + (255 * Min(b, 1.f))); }
	};

	// vectors
	class vec2 // adapted from https://github.com/dcow/RayTracer
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { float x, y; }; float cell[2]; };
#pragma warning (pop)
		vec2() : x(0), y(0) {}
		vec2(float v) : x(v), y(v) {}
		vec2(float x, float y) : x(x), y(y) {}
		vec2 operator - () const { return vec2(-x, -y); }
		vec2 operator + (const vec2& addOperand) const { return vec2(x + addOperand.x, y + addOperand.y); }
		vec2 operator - (const vec2& operand) const { return vec2(x - operand.x, y - operand.y); }
		vec2 operator * (const vec2& operand) const { return vec2(x * operand.x, y * operand.y); }
		vec2 operator * (float operand) const { return vec2(x * operand, y * operand); }
		void operator -= (const vec2& a) { x -= a.x; y -= a.y; }
		void operator += (const vec2& a) { x += a.x; y += a.y; }
		void operator *= (const vec2& a) { x *= a.x; y *= a.y; }
		void operator *= (float a) { x *= a; y *= a; }
		float& operator [] (const int idx) { return cell[idx]; }
		float length() { return betterSqrt(x * x + y * y); }
		float sqrLentgh() { return x * x + y * y; }
		vec2 normalized() { float r = 1.0f / length(); return vec2(x * r, y * r); }
		void normalize() { float r = 1.0f / length(); x *= r; y *= r; }
		static vec2 normalize(vec2 v) { return v.normalized(); }
		float dot(const vec2& operand) const { return x * operand.x + y * operand.y; }
	};
	class vec3
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { float x, y, z, dummy; }; float cell[4]; };
#pragma warning (pop)
		vec3() : x(0), y(0), z(0) {};
		vec3(float v) : x(v), y(v), z(v) {}
		vec3(float c[3]) : x(c[0]), y(c[1]), z(c[2]) {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		vec3 operator - () const { return vec3(-x, -y, -z); }
		vec3 operator + (vec3 const& addOperand) const { return vec3(x + addOperand.x, y + addOperand.y, z + addOperand.z); }
		vec3 operator + (float const& addOperand) const { return vec3(x + addOperand, y + addOperand, z + addOperand); }
		vec3 operator - (vec3 const& operand) const { return vec3(x - operand.x, y - operand.y, z - operand.z); }
		vec3 operator * (vec3 const& operand) const { return vec3(x * operand.x, y * operand.y, z * operand.z); }
		vec3 operator / (vec3 const& operand) const { return vec3(x / operand.x, y / operand.y, z / operand.z); }
		bool operator > (const float& operand) const { return  x > operand && y > operand && z > operand; }
		bool operator < (const float& operand) const { return  x < operand&& y < operand&& z < operand; }
		bool operator < (const vec3& operand) const { return  x < operand.x && y < operand.y&& z < operand.z; }
		void operator -= (const vec3& a) { x -= a.x; y -= a.y; z -= a.z; }
		void operator += (const vec3& a) { x += a.x; y += a.y; z += a.z; }
		void operator *= (const vec3& a) { x *= a.x; y *= a.y; z *= a.z; }
		void operator *= (const float a) { x *= a; y *= a; z *= a; }
		float operator [] (const uint& idx) const { return cell[idx]; }
		float& operator [] (const uint& idx) { return cell[idx]; }
		float length() const { return betterSqrt(sqrLength()); }
		float sqrLength() const { return x * x + y * y + z * z; }
		vec3 normalized() const { float r = 1.0f / length(); return vec3(x * r, y * r, z * r); }
		void normalize() { float r = 1.0f / length(); x *= r; y *= r; z *= r; }
		vec3 cross(const vec3& operand) const { return vec3(y * operand.z - z * operand.y, z * operand.x - x * operand.z, x * operand.y - y * operand.x); }
		float dot(const vec3& operand) const { return x * operand.x + y * operand.y + z * operand.z; }
	};
	class vec4
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { float x, y, z, w; }; struct { vec3 xyz; float w2; }; float cell[4]; };
#pragma warning (pop)
		vec4() : x(0), y(0), z(0), w(0) {}
		vec4(float v) : x(v), y(v), z(v), w(v) {}
		vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		vec4(vec3 a, float b) : x(a.x), y(a.y), z(a.z), w(b) {}
		vec4 operator - () const { return vec4(-x, -y, -z, -w); }
		vec4 operator + (const vec4& addOperand) const { return vec4(x + addOperand.x, y + addOperand.y, z + addOperand.z, w + addOperand.w); }
		vec4 operator - (const vec4& operand) const { return vec4(x - operand.x, y - operand.y, z - operand.z, w - operand.w); }
		vec4 operator * (const vec4& operand) const { return vec4(x * operand.x, y * operand.y, z * operand.z, w * operand.w); }
		void operator -= (const vec4& a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; }
		void operator += (const vec4& a) { x += a.x; y += a.y; z += a.z; w += a.w; }
		void operator *= (const vec4& a) { x *= a.x; y *= a.y; z *= a.z; w *= a.w; }
		void operator *= (float a) { x *= a; y *= a; z *= a; w *= a; }
		float& operator [] (const int idx) { return cell[idx]; }
		float operator [] (const uint& idx) const { return cell[idx]; }
		float length() { return betterSqrt(x * x + y * y + z * z + w * w); }
		float sqrLentgh() { return x * x + y * y + z * z + w * w; }
		vec4 normalized() { float r = 1.0f / length(); return vec4(x * r, y * r, z * r, w * r); }
		void normalize() { float r = 1.0f / length(); x *= r; y *= r; z *= r; w *= r; }
		static vec4 normalize(vec4 v) { return v.normalized(); }
		float dot(const vec4& operand) const { return x * operand.x + y * operand.y + z * operand.z + w * operand.w; }
	};
	class uint4
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { uint x, y, z, w; }; uint cell[4]; };
#pragma warning (pop)

		uint4() : x(0), y(0), z(0), w(0) {}
		uint4(int v) : x(v), y(v), z(v), w(v) {}
		uint4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
		uint4 operator + (const uint4& addOperand) const { return uint4(x + addOperand.x, y + addOperand.y, z + addOperand.z, w + addOperand.w); }
		uint4 operator - (const uint4& operand) const { return uint4(x - operand.x, y - operand.y, z - operand.z, w - operand.w); }
		uint4 operator * (const uint4& operand) const { return uint4(x * operand.x, y * operand.y, z * operand.z, w * operand.w); }
		uint4 operator * (uint operand) const { return uint4(x * operand, y * operand, z * operand, w * operand); }
		void operator -= (const uint4& a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; }
		void operator += (const uint4& a) { x += a.x; y += a.y; z += a.z; w += a.w; }
		void operator *= (const uint4& a) { x *= a.x; y *= a.y; z *= a.z; w *= a.w; }
		void operator *= (uint a) { x *= a; y *= a; z *= a; w *= a; }
		uint& operator [] (const int idx) { return cell[idx]; }
	};
	class int4
	{
	public:
#pragma warning (push)
#pragma warning (disable:4201)
		union { struct { int x, y, z, w; }; int cell[4]; };
#pragma warning (pop)
		int4() : x(0), y(0), z(0), w(0) {}
		int4(int v) : x(v), y(v), z(v), w(v) {}
		int4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
		int4 operator - () const { return int4(-x, -y, -z, -w); }
		int4 operator + (const int4& addOperand) const { return int4(x + addOperand.x, y + addOperand.y, z + addOperand.z, w + addOperand.w); }
		int4 operator - (const int4& operand) const { return int4(x - operand.x, y - operand.y, z - operand.z, w - operand.w); }
		int4 operator * (const int4& operand) const { return int4(x * operand.x, y * operand.y, z * operand.z, w * operand.w); }
		int4 operator * (int operand) const { return int4(x * operand, y * operand, z * operand, w * operand); }
		void operator -= (const int4& a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; }
		void operator += (const int4& a) { x += a.x; y += a.y; z += a.z; w += a.w; }
		void operator *= (const int4& a) { x *= a.x; y *= a.y; z *= a.z; w *= a.w; }
		void operator *= (int a) { x *= a; y *= a; z *= a; w *= a; }
		int& operator [] (const int idx) { return cell[idx]; }
	};
	class mat4
	{
	public:
		mat4();
		float cell[16];
		float& operator [] (const int idx) { return cell[idx]; }
		mat4 operator * (const mat4 value);
		static mat4 identity();
		static mat4 translate(vec3 v);
		static mat4 rotate(vec3 v, float a);
		static mat4 rotatex(const float a);
		static mat4 rotatey(const float a);
		static mat4 rotatez(const float a);
		void invert()
		{
			// from MESA, via http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
			const float inv[16] = {
				 cell[5] * cell[10] * cell[15] - cell[5] * cell[11] * cell[14] - cell[9] * cell[6] * cell[15] +
				 cell[9] * cell[7] * cell[14] + cell[13] * cell[6] * cell[11] - cell[13] * cell[7] * cell[10],
				-cell[1] * cell[10] * cell[15] + cell[1] * cell[11] * cell[14] + cell[9] * cell[2] * cell[15] -
				 cell[9] * cell[3] * cell[14] - cell[13] * cell[2] * cell[11] + cell[13] * cell[3] * cell[10],
				 cell[1] * cell[6] * cell[15] - cell[1] * cell[7] * cell[14] - cell[5] * cell[2] * cell[15] +
				 cell[5] * cell[3] * cell[14] + cell[13] * cell[2] * cell[7] - cell[13] * cell[3] * cell[6],
				-cell[1] * cell[6] * cell[11] + cell[1] * cell[7] * cell[10] + cell[5] * cell[2] * cell[11] -
				 cell[5] * cell[3] * cell[10] - cell[9] * cell[2] * cell[7] + cell[9] * cell[3] * cell[6],
				-cell[4] * cell[10] * cell[15] + cell[4] * cell[11] * cell[14] + cell[8] * cell[6] * cell[15] -
				 cell[8] * cell[7] * cell[14] - cell[12] * cell[6] * cell[11] + cell[12] * cell[7] * cell[10],
				 cell[0] * cell[10] * cell[15] - cell[0] * cell[11] * cell[14] - cell[8] * cell[2] * cell[15] +
				 cell[8] * cell[3] * cell[14] + cell[12] * cell[2] * cell[11] - cell[12] * cell[3] * cell[10],
				-cell[0] * cell[6] * cell[15] + cell[0] * cell[7] * cell[14] + cell[4] * cell[2] * cell[15] -
				 cell[4] * cell[3] * cell[14] - cell[12] * cell[2] * cell[7] + cell[12] * cell[3] * cell[6],
				 cell[0] * cell[6] * cell[11] - cell[0] * cell[7] * cell[10] - cell[4] * cell[2] * cell[11] +
				 cell[4] * cell[3] * cell[10] + cell[8] * cell[2] * cell[7] - cell[8] * cell[3] * cell[6],
				 cell[4] * cell[9] * cell[15] - cell[4] * cell[11] * cell[13] - cell[8] * cell[5] * cell[15] +
				 cell[8] * cell[7] * cell[13] + cell[12] * cell[5] * cell[11] - cell[12] * cell[7] * cell[9],
				-cell[0] * cell[9] * cell[15] + cell[0] * cell[11] * cell[13] + cell[8] * cell[1] * cell[15] -
				 cell[8] * cell[3] * cell[13] - cell[12] * cell[1] * cell[11] + cell[12] * cell[3] * cell[9],
				 cell[0] * cell[5] * cell[15] - cell[0] * cell[7] * cell[13] - cell[4] * cell[1] * cell[15] +
				 cell[4] * cell[3] * cell[13] + cell[12] * cell[1] * cell[7] - cell[12] * cell[3] * cell[5],
				-cell[0] * cell[5] * cell[11] + cell[0] * cell[7] * cell[9] + cell[4] * cell[1] * cell[11] -
				 cell[4] * cell[3] * cell[9] - cell[8] * cell[1] * cell[7] + cell[8] * cell[3] * cell[5],
				-cell[4] * cell[9] * cell[14] + cell[4] * cell[10] * cell[13] + cell[8] * cell[5] * cell[14] -
				 cell[8] * cell[6] * cell[13] - cell[12] * cell[5] * cell[10] + cell[12] * cell[6] * cell[9],
				 cell[0] * cell[9] * cell[14] - cell[0] * cell[10] * cell[13] - cell[8] * cell[1] * cell[14] +
				 cell[8] * cell[2] * cell[13] + cell[12] * cell[1] * cell[10] - cell[12] * cell[2] * cell[9],
				-cell[0] * cell[5] * cell[14] + cell[0] * cell[6] * cell[13] + cell[4] * cell[1] * cell[14] -
				 cell[4] * cell[2] * cell[13] - cell[12] * cell[1] * cell[6] + cell[12] * cell[2] * cell[5],
				 cell[0] * cell[5] * cell[10] - cell[0] * cell[6] * cell[9] - cell[4] * cell[1] * cell[10] +
				 cell[4] * cell[2] * cell[9] + cell[8] * cell[1] * cell[6] - cell[8] * cell[2] * cell[5]
			};
			const float det = cell[0] * inv[0] + cell[1] * inv[4] + cell[2] * inv[8] + cell[3] * inv[12];
			if (det != 0)
			{
				const float invdet = 1.0f / det;
				for (int i = 0; i < 16; i++) cell[i] = inv[i] * invdet;
			}
		}
	};
	using point3 = vec3;

	inline vec3 Min(vec3 const& a, vec3 const& b) { return vec3(a.x > b.x ? b.x : a.x, a.y > b.y ? b.y : a.y, a.z > b.z ? b.z : a.z); }
	inline vec3 Max(vec3 const& a, vec3 const& b) { return vec3(a.x < b.x ? b.x : a.x, a.y < b.y ? b.y : a.y, a.z < b.z ? b.z : a.z); }
	vec3 operator * (const float& s, const vec3& v);
	vec3 operator * (const vec3& v, const float& s);
	color operator * (const float& s, const color& v);
	color operator * (const color& v, const float& s);
	color operator + (const vec3& v, const color& s);
	color operator + (const color& s, const vec3& v);
	vec4 operator * (const float& s, const vec4& v);
	vec4 operator * (const vec4& v, const float& s);
	vec4 operator * (const vec4& v, const mat4& M);
	vec3 operator / (const float& v, const vec3& M);
	static int s_Seed = (int)time(NULL);

	inline float frand()
	{
		int* seed = &s_Seed;
		union
		{
			float fres;
			uint ires;
		};

		seed[0] *= 16807;
		ires = ((((uint)seed[0]) >> 9) | 0x3f800000);
		return fres - 1.0f;
	}

	inline int irand(uint64_t min, uint64_t max)
	{
		__int64 v = 2;
		v ^= s_Seed;
		v = v * 3935559000370003845ULL + 2691343689449507681ULL;
		v ^= v >> 21; v ^= v << (__int64)37; v ^= v >> 4;
		v *= 2685821657736338717ULL;

		return (int)floor((v >> 32) * (max - min) * (2.328306436539e-10) + min); //1/((2^32))
	}

	inline vec3 RandomUnitSphere()
	{
		while (true)
		{
			Util::vec3 p = 2.f * Util::vec3(frand(), frand(), frand()) - Util::vec3(1.f, 1.f, 1.f);
			if (p.sqrLength() >= 1) continue;
			return p;
		}
	}

	inline vec3 RandomUnitDisk() {
		while (true) {
			Util::vec3 p = Util::vec3(Util::frand(), Util::frand(), 0) - Util::vec3(1.f, 1.f, 0);
			if (p.sqrLength() >= 1) continue;
			return p;
		}
	}


}