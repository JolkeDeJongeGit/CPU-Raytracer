#include "precomp.h"
#include "Camera.h"
namespace rtc
{
	Camera::Camera(float a_Fov, Util::point3 a_LookAt, Util::point3 a_LookFrom, float a_FocusDist, float a_Aperture)
		: m_Fov(a_Fov), m_Translation(a_LookFrom), m_LookAt(a_LookAt)
	{
		// Calculating the fov to the new viewport witdh and height
		float fovRads = Util::DegreeToRads(m_Fov);
		float viewportHeight = 2.0f * tanf(fovRads *0.5f);
		float viewportWidth = aspectRatio * viewportHeight;

		// We rotate the local axis of the camera 
		m_XLocalAxis = (a_LookFrom - a_LookAt).normalized(); // returns the direction where the camera is looking at
		m_ZLocalAxis = Util::vec3(0.f, 1.f, 0.f).cross(m_XLocalAxis).normalized(); // returns the perpendicular value between the y and x value 
		m_YLocalAxis = m_XLocalAxis.cross(m_ZLocalAxis); // returns the perpendicular value between the x and z so we get y value

		// Setting base values
		m_ViewportHeight = a_FocusDist * viewportHeight * m_YLocalAxis;
		m_ViewportWidth = a_FocusDist * viewportWidth * m_ZLocalAxis;

		// Set the value on the top side of the screen based on the viewport
		m_TopCorner = m_Translation + m_ViewportHeight * 0.5f - m_ViewportWidth * 0.5f - a_FocusDist*m_XLocalAxis;
		m_Aperture = a_Aperture;
		m_LensRadius = a_Aperture * 0.5f;
		m_FocusDist = a_FocusDist;
	}
}
