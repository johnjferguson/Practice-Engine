#pragma once
#include "Window.h"
#include "IntervalTimer.h"

class Engine
{
public:
	Engine();
	int Start();
private:
	void DoFrame();
private:
	Window wnd;
	IntervalTimer timer;
};