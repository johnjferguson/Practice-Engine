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
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	std::optional<int> ProcessMessages();
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int width;
	int height;
	// width and height have to be defined before mouse
	Mouse mouse;
	Keyboard kbd;
	HWND hWnd;
	std::unique_ptr<Graphics> pGraphics;
};