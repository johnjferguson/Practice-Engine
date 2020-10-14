#pragma once
#include "LeanWindows.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <optional>
#include "Graphics.h"
#include <memory>
#include <unordered_map>

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
		R480X640,
		FULLSCREEN
	};
	enum class Style
	{
		INVALID,
		WINDOWED,
		BORDERLESS
	};
	struct Dimension
	{
		int width;
		int height;
	};
public:
	Window(Window::Resolution resolution, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	std::optional<int> ProcessMessages();
	Graphics& Gfx();
	void SetResolution(Window::Resolution resolution_in);
	Window::Resolution GetResolution() const;
	Window::Dimension GetDimension(Window::Resolution resolution) const;
	void SetStyle(Window::Style style_in);
	Window::Style GetStyle() const;
	HWND GetHWnd();
private:
	static LRESULT CALLBACK HandleThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Mouse mouse;
	Keyboard kbd;
private:
	Window::Resolution resolution;
	Window::Style style;
	HWND hWnd;
	std::unique_ptr<Graphics> m_graphics;
};