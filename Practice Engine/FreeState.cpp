#include "FreeState.h"
#include "MenuState.h"

FreeState FreeState::freeState;

void FreeState::Init()
{
}

void FreeState::Cleanup()
{
}

void FreeState::Pause()
{
}

void FreeState::Resume()
{
}

void FreeState::HandleEvents(Engine& engine)
{
	Window* wnd = GetWindow(engine);
	while (!wnd->kbd.IsEmpty())
	{
		auto e = wnd->kbd.PopEvent();
		switch (e.GetType())
		{
		case Keyboard::Event::Type::Press:
			if (e.GetCharacter() == VK_ESCAPE)
			{
				engine.PushState(MenuState::Instance());
			}
			break;
		default:
			break;
		}
	}
}

void FreeState::Update(Engine& engine)
{
}

void FreeState::Render(Engine& engine)
{
	engine.DoFrame();
}
