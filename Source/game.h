#pragma once
#include "Tile.h"
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"

namespace Template {

class Surface;
class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );

	//void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	//void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	//void MouseMove(int x, int y);
	//void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown(int key);
private:
	Surface* screen{ nullptr };
	rtc::Renderer m_Renderer;
	float m_MoveSpeed = .5f;
	float m_RotateSpeed = .5f;
};

}; // namespace Template