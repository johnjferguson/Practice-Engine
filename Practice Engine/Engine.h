#pragma once
#include "Window.h"
#include "IntervalTimer.h"
#include "ImguiManager.h"
#include <vector>

class EngineState;

class Engine
{
	// TODO: implement a better way to provide children with private access
	friend class EngineState;
	friend class MenuState;
	friend class FreeState;
public:
	Engine();
	int Start();
	// might move to private and make state a friend class
	void ChangeState(EngineState* pState);
	void PushState(EngineState* pState);
	void PopState();
private:
	Window& Wnd();
	void DoFrame();
	void ToggleImguiDemoWindow();
	void ShowImguiDemoWindow();
	void ChangeWindowResolution(Window::Resolution resolution);
private:
	const wchar_t* windowName;
	// imgui needs to be before Window
	ImguiManager imgui;
	std::unique_ptr<Window> pWnd;
	IntervalTimer timer;
	// all engine states are singletons so raw pointers are fine
	std::vector<EngineState*> states;
	// states
	bool imguiDemoEnabled = false;
};