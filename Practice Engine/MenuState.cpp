#include "MenuState.h"
#include "ImGui/imgui.h"
#include <sstream>

MenuState MenuState::menuState;

void MenuState::Init()
{
}

void MenuState::Cleanup()
{
}

void MenuState::Pause()
{
}

void MenuState::Resume()
{
}

void MenuState::HandleEvents(Engine& engine)
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
				engine.PopState();
			}
			break;
		default:
			break;
		}
	}
}

void MenuState::Update(Engine& engine)
{
}

void MenuState::Render(Engine& engine)
{
	engine.DoFrame();
	ImGui::Text("Menu very cool");
	if (ImGui::Button("Quit", { 0,0 }))
	{
		PostQuitMessage(0);
	}
	if (ImGui::Button("Toggle Demo Window", { 0,0 }))
	{
		engine.ToggleImguiDemoWindow();
	}

	if (ImGui::RadioButton("640 x 480", engine.Wnd().GetResolution() == Window::Resolution::R640X480))
	{
		engine.Wnd().SetStyle(Window::Style::WINDOWED);
		engine.Wnd().SetResolution(Window::Resolution::R640X480);
	}
	else if (ImGui::RadioButton("480 x 640", engine.Wnd().GetResolution() == Window::Resolution::R480X640))
	{
		engine.Wnd().SetStyle(Window::Style::WINDOWED);
		engine.Wnd().SetResolution(Window::Resolution::R480X640);
	}
	else if (ImGui::RadioButton("1280 x 720", engine.Wnd().GetResolution() == Window::Resolution::R1280X720))
	{
		engine.Wnd().SetStyle(Window::Style::WINDOWED);
		engine.Wnd().SetResolution(Window::Resolution::R1280X720);
	}
	else if (ImGui::RadioButton("1920 x 1080", engine.Wnd().GetResolution() == Window::Resolution::R1920X1080))
	{
		engine.Wnd().SetStyle(Window::Style::WINDOWED);
		engine.Wnd().SetResolution(Window::Resolution::R1920X1080);
	}
	else if (ImGui::RadioButton("Fullscreen", engine.Wnd().GetResolution() == Window::Resolution::FULLSCREEN))
	{
		engine.Wnd().SetStyle(Window::Style::BORDERLESS);
		engine.Wnd().SetResolution(Window::Resolution::FULLSCREEN);
	}
	
	RECT rect;
	GetWindowRect(engine.Wnd().GetHWnd(), &rect);
	std::stringstream ss;
	ss << "left: " << rect.left << std::endl << "right: " << rect.right << std::endl << "top: " << rect.top << std::endl << "bottom: " << rect.bottom << std::endl
		<< "width: " << rect.right - rect.left << std::endl << "height: " << rect.bottom - rect.top;

	ImGui::Text(ss.str().c_str());
}
