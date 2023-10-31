#pragma once
#include "Scene.h"
#include "Tile.h"
namespace Util { class Ray; }
namespace rtc
{

	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		Util::color ColorRender(Util::Ray const& a_Ray, int a_Bounces,  HitData& a_hitData);
		Util::color LightRender(HitData& a_HitData);
		Util::color NormalRender(Util::Ray const& a_Ray);
		void RenderScene();
		void ResetBuffer();
		rtc::Scene m_Scene;

		// Debug Information
		Template::timer m_FrameTimer;
		Template::timer m_DoneTimer;
		int m_RayShot = 0;
		int m_CurrentSample = 0;
		int m_MaxSample = 50;
		float m_FrameElapsed = 0;
		float m_DoneElapsed = 0;
		bool m_MoveRender = false;
		Util::Pixel* m_PixelBuffer;
	private:
		void ThreadLoop();

		Util::color* m_ColorBuffer;
		Util::color m_Background;

		std::vector<std::unique_ptr<rtc::Tile>> m_TileQueue;
		std::vector<std::thread> m_Threads;
		std::atomic<int> m_nextTile{ 0 };

		std::mutex m_Mtx;
		std::condition_variable m_Cv;

		bool m_ThreadRunning = true;
		const int m_amountThreads = (int)std::thread::hardware_concurrency() - 1;
	};
}

