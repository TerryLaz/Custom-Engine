// ELEFTERIOS LAZARIDIS
#pragma once
#include "WindowManager.h"
#include "GraphicsManager.h"

class Engine
{
private:
	WindowManager window;
	GraphicsManager graphics;
	Timer timer;

public:
	bool Initialize(HINSTANCE hInstance, int width, int height);
	bool ProcessMessages();
	void Update();
	void Render();
};