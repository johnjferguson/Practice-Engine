#include "Window.h"
#include <assert.h>
#include <sstream>
#include "MessageMap.h"
#include "Assert.h"
#include "resource.h"
#include "ImGui/examples/imgui_impl_win32.h"

// initialize singleton
Window::WindowClass Window::WindowClass::wndClass;

const wchar_t * Window::WindowClass::GetName() 
{
	return name;
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInstance;
}

Window::WindowClass::WindowClass()
	:
	hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleThunk;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr; // static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hIconSm = nullptr; // static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = name;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	// clean up window class
	UnregisterClass(name, hInstance);
}


Window::Window(Window::Resolution resolution, const wchar_t * name)
	:
	mouse(GetDimension(resolution).width, GetDimension(resolution).height),
	resolution(resolution)
{
	const Dimension dimension = GetDimension(resolution);
	const Dimension monitor_dimension = GetDimension(Window::Resolution::FULLSCREEN);

	// center the window in the monitor
	RECT rect;
	rect.left = (monitor_dimension.width - dimension.width) / 2;
	rect.top = (monitor_dimension.height - dimension.height) / 2;
	rect.right  = dimension.width  + rect.left;
	rect.bottom = dimension.height + rect.top;

	hWnd = CreateWindowEx(
		0, WindowClass::GetName(),
		L"Practice Engine Window",
		0,
		rect.left, rect.top, dimension.width, dimension.height,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	ASSERT_LAST_ERROR();

	style = Window::Style::WINDOWED;
	m_graphics = std::make_unique<Graphics>(hWnd, style == Window::Style::WINDOWED, dimension.width, dimension.height);
	SetResolution(resolution);
	
	// set window api information to be able to use member functions
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	// make window visable
	ShowWindow(hWnd, SW_SHOW);

	// init imgui for win32
	ImGui_ImplWin32_Init(hWnd);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
	ProcessMessages();
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return int(msg.wParam);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics & Window::Gfx()
{
	return *m_graphics;
}

Window::Resolution Window::GetResolution() const
{
	return resolution;
}

void Window::SetResolution(Window::Resolution resolution_in)
{
	Dimension dimension = GetDimension(resolution_in);
	Dimension monitor_dimension = GetDimension(Window::Resolution::FULLSCREEN);

	HWND hWndInsertAfter = 0;
	UINT windowStyle = 0;

	switch (style)
	{
	case Window::Style::WINDOWED:
		hWndInsertAfter = HWND_NOTOPMOST;
		windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_SYSMENU | WS_THICKFRAME);
		break;
	case Window::Style::BORDERLESS:
		hWndInsertAfter = HWND_TOPMOST;
		windowStyle = WS_POPUPWINDOW;
		break;
	default:
		assert(false && "Window::SetResolution() unhandled style.");
		break;
	}

	RECT rect;
	rect.left = (monitor_dimension.width - dimension.width) / 2;
	rect.right = rect.left + dimension.width;
	rect.top = (monitor_dimension.height - dimension.height) / 2;
	rect.bottom = rect.top + dimension.height;

	assert(rect.left >= 0 && rect.top >= 0 && "Window::SetResolution() bad resolution");

	AdjustWindowRect(&rect, windowStyle, false);

	SetWindowLongPtr(hWnd, GWL_STYLE, windowStyle);

	bool b = SetWindowPos(hWnd, hWndInsertAfter,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);


	GetWindowRect(hWnd, &rect);

	int j = 0;
	// re-create the graphics object since buffer size has changed
	m_graphics->WindowSizeChange(hWnd, dimension.width, dimension.height, style == Window::Style::WINDOWED);

	// set window dimensions for the mouse so normal will be correct
	mouse.SetWindowDimension(dimension.width, dimension.height);
	
	resolution = resolution_in;

	ShowWindow(hWnd, SW_SHOW);
}

void Window::SetStyle(Window::Style style_in)
{
	if (style_in != style)
	{
		style = style_in;
		SetResolution(resolution);
	}
}

Window::Style Window::GetStyle() const
{
	return style;
}

HWND Window::GetHWnd()
{
	return hWnd;
}

LRESULT Window::HandleThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->Handle(hWnd, msg, wParam, lParam);
}

LRESULT Window::Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	#if _DEBUG
	static MessageMap mm;
	OutputDebugString(mm(msg, wParam, lParam).c_str());
	#endif

	POINTS pt = MAKEPOINTS(lParam);

	switch (msg)
	{
	// shuts down program when exit button is pressed
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		mouse.OnMove(pt.x, pt.y);
		break;
	case WM_LBUTTONDOWN:
		mouse.OnLeftPress(pt.x, pt.y);
		break;
	case WM_LBUTTONUP:
		mouse.OnLeftRelease(pt.x, pt.y);
		break;
	case WM_RBUTTONDOWN:
		mouse.OnRightPress(pt.x, pt.y);
		break;
	case WM_RBUTTONUP:
		mouse.OnRightRelease(pt.x, pt.y);
		break;
	case WM_KEYDOWN:
		kbd.OnKeyPress(char(wParam));
		break;
	case WM_KEYUP:
		kbd.OnKeyRelease((unsigned char)wParam);
		break;
	case WM_GETMINMAXINFO:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Dimension Window::GetDimension(Window::Resolution resolution) const
{
	HMONITOR hmon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfoW(hmon, &mi);

	switch (resolution)
	{
	case Window::Resolution::R1920X1080:
		return { 1920, 1080 };
		break;
	case Window::Resolution::R1280X720:
		return { 1280, 720 };
		break;
	case Window::Resolution::R640X480:
		return { 640, 480 };
		break;
	case Window::Resolution::R480X640:
		return { 480, 640 };
		break;
	case Window::Resolution::FULLSCREEN:
		return { mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top };
		break;
	default:
		return { 0, 0 };
		break;
	}
}

/*
void Window::SetFullscreen(bool fullscreen)
{
	if (m_Fullscreen != fullscreen)
	{
		m_Fullscreen = fullscreen;

		if (m_Fullscreen) // Switching to fullscreen.
		{
			// Store the current window dimensions so they can be restored 
			// when switching out of fullscreen state.
			::GetWindowRect(m_hWindow, &m_WindowRect);

			// Set the window style to a borderless window so the client area fills
			// the entire screen.
			UINT windowStyle = WS_OVERLAPPEDWINDOW &~(WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU | WS_THICKFRAME);
			// Is this the same thing?
			UINT sytleDiff = windowStyle & ~WS_POPUP;

			::SetWindowLongW(m_hWindow, GWL_STYLE, windowStyle);

			// Query the name of the nearest display device for the window.
			// This is required to set the fullscreen dimensions of the window
			// when using a multi-monitor setup.
			HMONITOR hMonitor = ::MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			// Get the settings for the primary display. These settings are used
			// to determine the correct position and size to position the window
			DEVMODE devMode = {};
			devMode.dmSize = sizeof(DEVMODE);
			::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

			::SetWindowPos(m_hWindow, HWND_TOPMOST,
				devMode.dmPosition.x,
				devMode.dmPosition.y,
				devMode.dmPosition.x + devMode.dmPelsWidth,
				devMode.dmPosition.y + devMode.dmPelsHeight,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWindow, SW_MAXIMIZE);
		}
		else
		{
			// Restore all the window decorators.
			::SetWindowLong(m_hWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(m_hWindow, HWND_NOTOPMOST,
				m_WindowRect.left,
				m_WindowRect.top,
				m_WindowRect.right - m_WindowRect.left,
				m_WindowRect.bottom - m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWindow, SW_NORMAL);
		}
	}

}
*/



