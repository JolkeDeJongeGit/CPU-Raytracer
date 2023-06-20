#pragma once
#include "Raytracer/Ray.h"
namespace rtc
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float a_Fov, Util::point3 a_LookAt, Util::point3 a_LookFrom, float a_FocusDist, float a_Aperture);
		inline Util::Ray GetRay(float a_XScreenNorm, float a_YScreenNorm) const
		{
			if (m_Aperture >= FLT_EPSILON)
			{
				Util::vec3 rd = m_LensRadius * Util::RandomUnitDisk();
				Util::vec3 offset = m_XLocalAxis * rd.x + m_YLocalAxis * rd.y;
				return Util::Ray(m_Translation + offset, (m_TopCorner + a_XScreenNorm * m_ViewportWidth - a_YScreenNorm * m_ViewportHeight - m_Translation - offset));
			}

			return Util::Ray(m_Translation, (m_TopCorner + a_XScreenNorm * m_ViewportWidth - a_YScreenNorm * m_ViewportHeight - m_Translation));
		};
		inline void TranslateCamera(Util::vec3 a_Translation)
		{
			m_Translation += a_Translation *0.2f;
			UpdateLookAt();
		}

		inline void UpdateLookAt()
		{
			// Calculating the fov to the new viewport witdh and height
			float fovRads = Util::DegreeToRads(m_Fov);
			float viewportHeight = 2.0f * tanf(fovRads / 2);
			float viewportWidth = aspectRatio * viewportHeight;

			// We rotate the local axis of the camera 
			m_XLocalAxis = (m_Translation - m_LookAt).normalized(); // returns the direction where the camera is looking at
			m_ZLocalAxis = Util::vec3(0.f, 1.f, 0.f).cross(m_XLocalAxis).normalized(); // returns the perpendicular value between the y and x value 
			m_YLocalAxis = m_XLocalAxis.cross(m_ZLocalAxis); // returns the perpendicular value between the x and z so we get y value

			// Setting base values
			m_ViewportHeight = m_FocusDist * viewportHeight * m_YLocalAxis;
			m_ViewportWidth = m_FocusDist * viewportWidth * m_ZLocalAxis;

			// Set the value on the top side of the screen based on the viewport
			m_TopCorner = m_Translation + m_ViewportHeight / 2 - m_ViewportWidth / 2 - m_FocusDist * m_XLocalAxis;
		}
		inline void RotateCamera(Util::vec3 a_Rotation)
		{
			m_LookAt = a_Rotation;
			UpdateLookAt();
		}
		Util::point3 m_LookAt;
		Util::vec3 m_XLocalAxis;
		Util::vec3 m_YLocalAxis;
		Util::vec3 m_ZLocalAxis;
	private:
		float m_Fov = 50.f;
		float m_LensRadius = 1.f;
		float m_Aperture = 0.f;
		float m_FocusDist = 0.f;

		Util::point3 m_Translation;
		Util::point3 m_TopCorner;

		Util::vec3 m_ViewportHeight;
		Util::vec3 m_ViewportWidth;
	};
}
