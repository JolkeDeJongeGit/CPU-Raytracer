#include "precomp.h"
#include "Util.h"
namespace Util
{
	color operator * (const float& s, const color& v) { return color(v.r * s, v.g * s, v.b * s); }
	color operator * (const color& v, const float& s) { return color(v.r * s, v.g * s, v.b * s); }
	color operator + (const vec3& v, const color& s){ return color(v.x + s.r, v.y + s.g, v.z + s.b); }
	color operator + (const color& s, const vec3& v){ return color(v.x + s.r, v.y + s.g, v.z + s.b); }
#pragma region Vector
	vec3 operator * (const float& s, const vec3& v) { return vec3(v.x * s, v.y * s, v.z * s); }
	vec3 operator * (const vec3& v, const float& s) { return vec3(v.x * s, v.y * s, v.z * s); }
	vec4 operator * (const float& s, const vec4& v) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
	vec4 operator * (const vec4& v, const float& s) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
	vec4 operator * (const vec4& v, const mat4& M)
	{
		vec4 mx(M.cell[0], M.cell[4], M.cell[8], M.cell[12]);
		vec4 my(M.cell[1], M.cell[5], M.cell[9], M.cell[13]);
		vec4 mz(M.cell[2], M.cell[6], M.cell[10], M.cell[14]);
		vec4 mw(M.cell[3], M.cell[7], M.cell[11], M.cell[15]);
		return v.x * mx + v.y * my + v.z * mz + v.w * mw;
	}

	vec3 operator/ (const float& v, const vec3& M)
	{
		return vec3(v / M.x, v / M.y, v / M.z);
	}

#pragma endregion
#pragma region Matrix
	mat4::mat4()
	{
		memset(cell, 0, 64);
		cell[0] = cell[5] = cell[10] = cell[15] = 1;
	}
	mat4 mat4::operator*(const mat4 value)
	{
		mat4 newMatrix;
		for (int x = 0; x < 4; x++) { // row number of output
			for (int y = 0; y < 4; y++) { // column number of output
				newMatrix[4 * x + y] = 0;
				for (int z = 0; z < 4; z++) { // four elements are added for this output
					newMatrix[4 * x + y] += cell[4 * x + z] * value.cell[4 * z + y];
				}
			}
		}
		return newMatrix;
	}
	mat4 mat4::identity()
	{
		mat4 r;
		memset(r.cell, 0, 64);
		r.cell[0] = r.cell[5] = r.cell[10] = r.cell[15] = 1.0f;
		return r;
	}
	mat4 mat4::translate(vec3 v)
	{
		mat4 M;
		M.cell[0] = 1; M.cell[1] = 0; M.cell[2] = 0; M.cell[3] = v.x;
		M.cell[4] = 0; M.cell[5] = 1; M.cell[6] = 0; M.cell[7] = v.y;
		M.cell[9] = 0; M.cell[9] = 0; M.cell[10] = 1; M.cell[11] = v.z;

		return M;
	}

	mat4 mat4::rotate(const vec3 l, const float a)
	{
		// http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation
		mat4 M;
		const float u = l.x, v = l.y, w = l.z, ca = cosf(a), sa = sinf(a);
		M.cell[0] = u * u + (v * v + w * w) * ca, M.cell[1] = u * v * (1 - ca) - w * sa;
		M.cell[2] = u * w * (1 - ca) + v * sa, M.cell[4] = u * v * (1 - ca) + w * sa;
		M.cell[5] = v * v + (u * u + w * w) * ca, M.cell[6] = v * w * (1 - ca) - u * sa;
		M.cell[8] = u * w * (1 - ca) - v * sa, M.cell[9] = v * w * (1 - ca) + u * sa;
		M.cell[10] = w * w + (u * u + v * v) * ca;
		M.cell[3] = M.cell[7] = M.cell[11] = M.cell[12] = M.cell[13] = M.cell[14] = 0, M.cell[15] = 1;
		return M;
	}
	mat4 mat4::rotatex(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[5] = ca, M.cell[6] = -sa;
		M.cell[9] = sa, M.cell[10] = ca;
		return M;
	}
	mat4 mat4::rotatey(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[0] = ca, M.cell[2] = sa;
		M.cell[8] = -sa, M.cell[10] = ca;
		return M;
	}
	mat4 mat4::rotatez(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[0] = ca, M.cell[1] = -sa;
		M.cell[4] = sa, M.cell[5] = ca;
		return M;
	}
#pragma endregion
}