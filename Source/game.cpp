#include "precomp.h"
#include "surface.h"
#include "game.h"
#include "DebugUI.h"
#include <iostream>

namespace Template
{
	timer t;
	float rayPerSec = 0;
	float deltaTimer = 0;
	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		t.init();
	}
	
	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
	}

	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		Util::DebugUi d = Util::DebugUi(screen, Util::vec2(1032, 16));
		screen->Clear(51);
		m_Renderer.RenderScene();

		for (int y = 0; y < imageHeight; y++)
		{
			memcpy(screen->GetBuffer() + (size_t)screen->GetWidth() * (size_t)y, m_Renderer.m_PixelBuffer + (size_t)imageWidth * (size_t)y, imageWidth * sizeof(Template::Pixel));
		}
		
		#pragma region Debug
			d.Print("Debug info:");
			d.Print("-----------------------------------------");
			d.Print("FPS: ", (float)(1000.f / m_Renderer.m_FrameElapsed));
			d.Print("Last frame: ", m_Renderer.m_FrameElapsed);
			d.Print("Program running: ", deltaTimer += deltaTime);
			d.Print("Amount of objects ",  m_Renderer.m_Scene.m_SceneList.size());
			d.Print("-----------------------------------------");
			d.Print("Current sample: ", m_Renderer.m_CurrentSample);
			d.Print("amount of samples: ", m_Renderer.m_MaxSample);
			d.Print("amount of Ray: ", (int)m_Renderer.m_RayShot);
			d.Print("Ray Per Seconds: ", (int)rayPerSec);
			d.Print("Time Done: ", m_Renderer.m_DoneElapsed / 1000.f, Util::color(1.f, 0, 0));
			d.Print("-----------------------------------------");
		#pragma endregion

		if (t.elapsed() >= 1000 && m_Renderer.m_DoneElapsed == 0)
		{
			rayPerSec = (float)m_Renderer.m_RayShot;
			m_Renderer.m_RayShot = 0;
			t.reset();
		}
	}

	void Game::KeyDown(int key)
	{
		if (key == 26) // W Key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(-m_Renderer.m_Scene.m_Camera.m_XLocalAxis * 0.5f );
		}

		if (key == 22) // S key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(m_Renderer.m_Scene.m_Camera.m_XLocalAxis  * 0.5f);
		}

		if (key == 4) // A Key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(-m_Renderer.m_Scene.m_Camera.m_ZLocalAxis * 0.5f);
		}

		if (key == 7) // D key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(m_Renderer.m_Scene.m_Camera.m_ZLocalAxis * 0.5f);
		}

		if (key == 44) // space key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(m_Renderer.m_Scene.m_Camera.m_YLocalAxis * 0.5f);
		}

		if (key == 224) // cntr key
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_Camera.TranslateCamera(-m_Renderer.m_Scene.m_Camera.m_YLocalAxis * 0.5f);
		}

		if (key == 46)
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_SceneID++;
			m_Renderer.m_Scene.LoadScene(m_Renderer.m_Scene.m_SceneID);
		}

		if (key == 45)
		{
			m_Renderer.m_MoveRender = true;
			m_Renderer.m_Scene.m_SceneID--;
			m_Renderer.m_Scene.LoadScene(m_Renderer.m_Scene.m_SceneID);
		}
	}
};