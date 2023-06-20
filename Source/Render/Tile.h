#pragma once
namespace rtc
{
	enum class TileState { AWAITING, PROGRESS, SAMPLEDONE, DONE };
	class Tile
	{
	public:
		Tile(Util::vec2 a_Index) : m_Index(a_Index) {}
		inline TileState GetState() {return m_currentState; };
		void SetState(TileState const& a_State) { m_currentState = a_State; };
		inline int GetX() { return (int)m_Index.x; } // current index of x
		inline int GetY() { return (int)m_Index.y; } // current index of y
		int m_currentSample = 0;
	private:
		Util::vec2 m_Index = (0.f);
		std::mutex mutex;
		TileState m_currentState = TileState::AWAITING;
	};
}

