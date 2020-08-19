#pragma once
#include "LeanWindows.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <optional>
#include "Graphics.h"
#include <memory>

class Window
{
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName();
		static HINSTANCE GetInstance();
	private:
		WindowClass();
		~WindowClass();
		// doesn't allow copies of singleton
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
	private:
		static WindowClass wndClass;
		static constexpr const wchar_t* name = L"Practice Engine Class";
		HINSTANCE hInstance;
	};
public:
	enum class Resolution
	{
		R1920X1080,
		R1280X720,
		R640X480,
		FULLSCREEN
	};
public:
	Window(Window::Resolution resolution, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	void SetFullscreen(bool fullscreen);
	Window& operator=(const Window&) = delete;
	std::optional<int> ProcessMessages();
	Graphics& Gfx();
	Window::Resolution GetResolution() const;
	HWND GetHWnd();
private:
	static LRESULT CALLBACK HandleThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Mouse mouse;
	Keyboard kbd;
private:
	Window::Resolution resolution;
	int width;
	int height;
	// width and height have to be defined before mouse
	HWND hWnd;
	std::unique_ptr<Graphics> pGraphics;

	// temp
	HWND m_hWindow;
	RECT m_WindowRect;
	uint32_t m_Width;
	uint32_t m_Height;
	bool m_Fullscreen = false;
};