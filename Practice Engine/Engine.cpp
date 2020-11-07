#include "Engine.h"
#include "ImGui/examples/imgui_impl_dx11.h"
#include "ImGui/examples/imgui_impl_win32.h"
#include "EngineState.h"
#include "FreeState.h"
#include "MenuState.h"
#include "Assert.h"
#include "StringId.h"
#include <utility>

Engine::Engine()
	:
	windowName(L"Practice Engine Window")
{
	pWnd = std::make_unique<Window>(Window::Resolution::R1280X720, windowName);

	//ImGui_ImplDX11_Init(Wnd().Gfx().GetDevice(), Wnd().Gfx().GetContext());
	//ImGui_ImplWin32_Init(Wnd().GetHWnd());
	ASSERT_LAST_ERROR();

	states.push_back(FreeState::Instance());
}

int Engine::Start()
{

	timer.Set(SID("dt"));
	std::optional<int> ecode;
	while (!(ecode = pWnd->ProcessMessages()))
	{
		//auto dt = timer.End(HashString("dt"));
		//timer.Set(HashString("dt"));


		Wnd().Gfx().BeginFrame();
		//ImGui::Text(std::to_string(dt*1000.0f).c_str());
		states.back()->HandleEvents(*this);
		states.back()->Update(*this);
		states.back()->Render(*this);
		Wnd().Gfx().EndFrame();
	}
	return ecode.value();
}

void Engine::ChangeState(EngineState* pState)
{
	// clean up current state
	if (!states.empty())
	{
		states.back()->Cleanup();
		states.pop_back();
	}
	// push back new state and initialize
	states.push_back(pState);
	states.back()->Init();
}

void Engine::PushState(EngineState* pState)
{
	// pause current state
	if (!states.empty())
		states.back()->Pause();

	// push bakc new state and initialize
	states.push_back(pState);
	states.back()->Init();
}

void Engine::PopState()
{
	// cleanup and pop current state
	if (!states.empty())
	{
		states.back()->Cleanup();
		states.pop_back();
	}

	// resume previous state
	if (!states.empty())
		states.back()->Resume();
}

void Engine::ToggleImguiDemoWindow()
{
	imguiDemoEnabled = !imguiDemoEnabled;
}

void Engine::ShowImguiDemoWindow()
{
	if (imguiDemoEnabled)
	{
		ImGui::ShowDemoWindow(&imguiDemoEnabled);
	}
}

void Engine::ChangeWindowResolution(Window::Resolution resolution)
{
	Wnd().SetResolution(resolution);
}

Window& Engine::Wnd()
{
	return *pWnd;
}

void Engine::DoFrame()
{
	const float c = 1.0f; //float(sin(timer.Peek("dt"))) / 2.0f + 0.5f;
	Wnd().Gfx().ClearBuffer(c, 0.5f, 0.5f);
	Wnd().Gfx().DrawTestTriangle();
	ShowImguiDemoWindow();
}
