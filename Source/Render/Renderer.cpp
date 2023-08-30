#include "precomp.h"
#include "Raytracer/Ray.h"
#include "Renderer.h"
#include "BaseObject.h"
#include "Raytracer/BVHNode.h"
#include "PointLight.h"
#include "Metal.h"
#include "Dielectric.h"
#include "Texture.h"

namespace rtc
{
	Renderer::Renderer()
	{
		m_Scene.LoadScene(sceneID);
		m_ColorBuffer = new Util::color[imageWidth * imageHeight];
		m_PixelBuffer = new Template::Pixel[imageWidth * imageHeight];
		m_PixelBufferBvh = new Template::Pixel[imageWidth * imageHeight];
		m_Background = Util::color(0.5f, 0.7f, 1.0f);
		//m_Background = Util::color(0.f, 0.f, 0.f);

		m_MaxSample = 10000;

		int tileCountY = imageHeight / tileSize;
		int tileCountX = imageWidth / tileSize;
		int tile = tileCountX * tileCountY;

		for (int y = 0, i = 0, ix = 0; y < tileCountY; y++)
		{
			for (int x = 0; x < tileCountX; x++, i++)
			{
				int fy = (ix / tileCountX) % tileCountY;
				int fx = (ix + (x < tileCountX / 2)) % tileCountX;

				fx += (ix >= tile) * (fy % 2 ? 1 : -1);

				m_TileQueue.emplace_back(std::make_unique<rtc::Tile>(Util::vec2((float)fx, (float)fy)));

				ix += 2;
			}
		}
#if MULTITHREADING
		for (int i = 0; i < m_amountThreads; i++)
			m_Threads.emplace_back(std::thread(&Renderer::ThreadLoop, this));
		m_DoneTimer.init();
		m_FrameTimer.init();
#else
		m_DoneTimer.init();
		m_FrameTimer.init();
		float scale = 1.0f / m_MaxSample;
		int yOffset = 0;
		float randomSample = Util::frand();
		float inverseH = 1.f / (imageHeight - 1);
		float inverseW = 1.f / (imageWidth - 1);
		HitData hitData;
		for (int y = 0; y < imageHeight; y++)
		{
			for (int x = 0; x < imageWidth; x++)
			{
				for (int m = 0; m < m_MaxSample; m++)
				{
					float normPosy = (float)(y + randomSample) * inverseH;
					float normPosx = (float)(x + randomSample) * inverseW;
					Util::Ray a_R = m_Scene.m_Camera.GetRay(normPosx, normPosy);
					m_ColorBuffer[x + yOffset] += ColorRender(a_R, 100, hitData);
					m_ColorBufferBvh[x + yOffset] = ColorRender(a_R, 100, hitData, true);
				}

				Util::color c = m_ColorBuffer[x + yOffset];

				c.r = sqrt(scale * c.r);
				c.g = sqrt(scale * c.g);
				c.b = sqrt(scale * c.b);

				m_PixelBuffer[x + yOffset] = c.Get();
				m_PixelBufferBvh[x + yOffset] = m_ColorBufferBvh[x + yOffset].Get();
			}
			yOffset += imageWidth;
		}
		m_FrameElapsed = m_FrameTimer.elapsed() / m_MaxSample;
		m_DoneElapsed = m_DoneTimer.elapsed();
#endif
	}

	Renderer::~Renderer()
	{
		m_ThreadRunning = false;
		m_Cv.notify_all();
		for (size_t i = 0; i < m_Threads.size(); i++)
			if (m_Threads[i].joinable())
				m_Threads[i].join();
	}

	Template::Sprite s(new Template::Surface("Assets/skybox.png"), 1);
	float k = 1.f / 255;
	float piDivide = 1.f / Util::PI;
	// -----------------------------------------------------------
	// Renders the color where the ray hits
	// -----------------------------------------------------------
	Util::color Renderer::ColorRender(Util::Ray const& a_Ray, int a_Bounces, HitData& a_hitData, bool a_BvhView)
	{
		m_RayShot++;
		
		Util::color lightAttentuation;
		Util::Ray scattered;
		Util::color attenuation;
		Util::color box;
		if (a_Bounces <= 0)
			return Util::color(0, 0, 0);

		if (!a_BvhView)
		{
			if (m_Scene.m_TreeNode.HitTree(a_Ray, 0.001f, FLT_MAX, a_hitData))
			{
				Util::color color = a_hitData.material.albedo;
				switch (a_hitData.material.type)
				{
				case MaterialType::LAMBERTIAN:
				{
					lightAttentuation = LightRender(a_hitData);

					switch (a_hitData.material.Ttype)
					{
					case TextureType::CHECKER:
						rtc::Texture t;
						color = t.CheckerTexture(a_hitData.pointIntersection, color, (0.f));
						break;
					}
					return color * lightAttentuation;
				}
				case MaterialType::METAL:	
				{
					HitData hitData = a_hitData;

					Metal mat = Metal(color, a_hitData.material.customValue);
					mat.Scatter(a_Ray, a_hitData, attenuation, scattered);
					return attenuation * ColorRender(scattered, a_Bounces - 1, hitData);
				}
				case MaterialType::DIELECTRIC:
				{
					HitData hitData = a_hitData;

					Dielectric mat = Dielectric(color, a_hitData.material.customValue);
					mat.Scatter(a_Ray, a_hitData, attenuation, scattered);
					return attenuation * ColorRender(scattered, a_Bounces - 1, hitData);
			}
				default:
					throw("Not existing Material!!");
					break;
				}
			}

			Util::point3 dir = a_Ray.m_Dir.normalized();
			float theta = acosf(-dir.y);
			float f = atan2f(-dir.z, dir.x) + Util::PI;

			float u = f * piDivide;
			float v = 1.f - (theta * piDivide);

			int width = s.GetWidth();
			int height = s.GetHeight();
			int i = (int)(u * (width - 1));
			int j = (int)(v * (height - 1));

			auto pixel = s.GetBuffer()[i + j * width];
			float r = ((pixel >> 16) & 0xFF) * k;
			float g = ((pixel >> 8) & 0xFF) * k;
			float b = ((pixel) & 0xFF) * k;

			return Util::color(r, g, b);
		}
		else
		{
			return Util::color(0.f, 0.5f, 0) * (a_hitData.depth * Util::InvLayers);
		}
	}

	Util::color Renderer::LightRender(HitData& a_HitData)
	{
		Util::color color = 0.f;
		for (size_t i = 0; i < m_Scene.m_LightList.size(); i++)
		{
			Util::vec3 dirLight = (m_Scene.m_LightList[i]->m_Pos - a_HitData.pointIntersection).normalized(); // Gives the direction on where my lighsource is
			Util::Ray shadowRay = Util::Ray(a_HitData.pointIntersection, dirLight); // creates a new shadow ray
			if (!m_Scene.m_TreeNode.HitTree(shadowRay, 0.001f, FLT_MAX, a_HitData))
			{
				float LdotN = Util::Max(dirLight.dot(a_HitData.normal), 0.f); // Based on where the norm is pointing at I get a value
				color += (m_Scene.m_LightList[i]->m_Intensity / dirLight.sqrLength() * m_Scene.m_LightList[i]->m_C * LdotN);
			}
			else if (a_HitData.material.type == MaterialType::DIELECTRIC)
			{
				float LdotN = Util::Max(dirLight.dot(a_HitData.normal), 0.f); // Based on where the norm is pointing at I get a value
				color += (m_Scene.m_LightList[i]->m_Intensity / dirLight.sqrLength() * m_Scene.m_LightList[i]->m_C * LdotN);
			}
		}
		return color;
	}

	// -----------------------------------------------------------
	// Renders the NORMAL view where the ray hits
	// -----------------------------------------------------------
	Util::color Renderer::NormalRender(Util::Ray const& a_Ray)
	{
		m_RayShot++;

		HitData hitData;
		Util::Ray scattered;
		Util::color attenuation;
		if (m_Scene.m_TreeNode.HitTree(a_Ray, 0.001f, FLT_MAX, hitData))
		{
			Util::vec3 N = hitData.normal;
			return 0.5f * (Util::color(N.x, N.y, N.z) + Util::color(1, 1, 1));
		}
		return Util::color(0, 0, 0);
	}

	// -----------------------------------------------------------
	// Renders the Tiles on the screenddd
	// -----------------------------------------------------------
	void Renderer::RenderScene()
	{
		for (size_t i = 0; i < m_TileQueue.size(); i++)
		{
			if (m_TileQueue[i]->GetState() != rtc::TileState::SAMPLEDONE)
				break;

			if (i >= m_TileQueue.size() - 1)
			{
				// Saved the time on how long axis frame cost
				m_FrameElapsed = m_FrameTimer.elapsed();
				m_FrameTimer.reset();
				m_CurrentSample = m_TileQueue[i]->m_currentSample;

				// Checks if every sample is done
				if (m_MaxSample <= m_CurrentSample && m_DoneElapsed == 0)
				{
					m_DoneElapsed = m_DoneTimer.elapsed();
					m_DoneTimer.reset();
				}

				if (m_MoveRender)
				{
					ResetBuffer();
					m_MoveRender = false;
				}

				for (size_t j = 0; j < m_TileQueue.size(); j++)
					m_TileQueue[j]->SetState(rtc::TileState::AWAITING);

				m_nextTile.store(0);
				m_Cv.notify_all();
			}
		}
	}

	void Renderer::ResetBuffer()
	{
		for (size_t i = 0; i < m_TileQueue.size(); i++)
		{
			m_TileQueue[i]->m_currentSample = 0;
			m_TileQueue[i]->SetState(rtc::TileState::AWAITING);
		}

		memset(m_ColorBuffer, 0, (imageWidth * imageHeight) * sizeof(Util::color));
	}


	// -----------------------------------------------------------
	// Loops thread and handles Tile loading
	// -----------------------------------------------------------
	void Renderer::ThreadLoop()
	{
		size_t currentTile = 0;
		while (m_ThreadRunning)
		{
			// Locks the next tile int until it is done adding
			// Locks is just waiting untill something is done
			if ((size_t)m_nextTile <= m_TileQueue.size() - 1)
				currentTile = m_nextTile.fetch_add(1);
			else
				currentTile = (int)m_TileQueue.size();

			if (m_TileQueue.size() <= currentTile)
			{
				// Adds the value and locks the thread until all threads are done
				std::unique_lock<std::mutex> lock(m_Mtx);
				m_Cv.wait(lock);
				continue;
			}

			// This handles if the tile is not being worked on if that is the case it will
			// redo the loop
			std::unique_ptr<rtc::Tile> const& tile = m_TileQueue[currentTile];

			if (tile->GetState() != rtc::TileState::AWAITING)
				continue;

			// Set the state to progress
			tile->SetState(rtc::TileState::PROGRESS);

			// Checks if the amount of sample has been hit
			if (tile->m_currentSample < m_MaxSample)
			{
				tile->m_currentSample++;
				int yOffset = 0;
				// Loops through axis tile and draws it 
				float scale = 1.f / tile->m_currentSample;
				float randomSample = Util::frand();
				float inverseH = 1.f / (imageHeight - 1);
				float inverseW = 1.f / (imageWidth - 1);
				for (int y = tileSize * tile->GetY(); y < tileSize * (tile->GetY() + 1); y++)
				{
					yOffset = y * imageWidth;
					for (int x = tileSize * tile->GetX(); x < tileSize * (tile->GetX() + 1); x++)
					{
						float normPosy = (float)(y + randomSample) * inverseH;
						float normPosx = (float)(x + randomSample) * inverseW;

						const Util::Ray ray = m_Scene.m_Camera.GetRay(normPosx, normPosy);
#if NORMALVIEW
						m_ColorBuffer[x + yOffset] += NormalRender(ray);

						Util::color c = m_ColorBuffer[x + yOffset];
						c.r = sqrt(scale * c.r);
						c.g = sqrt(scale * c.g);
						c.b = sqrt(scale * c.b);

						m_PixelBuffer[x + yOffset] = c.Get();
#else
						HitData hitData;
						m_ColorBuffer[x + yOffset] += ColorRender(ray, 5, hitData);

						Util::color c = m_ColorBuffer[x + yOffset];
						c.r = sqrtf(scale * c.r);
						c.g = sqrtf(scale * c.g);
						c.b = sqrtf(scale * c.b);

						m_PixelBuffer[x + yOffset] = c.Get();
						m_PixelBufferBvh[x + yOffset] = ColorRender(ray, 1, hitData, true).Get();
#endif
					}
				}
				tile->SetState(rtc::TileState::SAMPLEDONE);
			}
			else
				tile->SetState(rtc::TileState::DONE);
		}
	}
}
