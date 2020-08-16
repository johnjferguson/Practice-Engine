#include "Engine.h"

Engine::Engine()
	:
	wnd(1280, 720, L"Practice Engine Window")
{}

int Engine::Start()
{
	timer.Set("dt");
	std::optional<int> ecode;
	while (!(ecode = wnd.ProcessMessages()))
	{
		DoFrame();
	}
	return ecode.value();
}

void Engine::DoFrame()
{
	const float c = float(sin(timer.Peek("dt"))) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, 0.5f, 0.5f);
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();

}
