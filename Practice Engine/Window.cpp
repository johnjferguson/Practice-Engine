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
	mouse(width, height),
	resolution(resolution)
{
	// get the screen resolution to center the window
	HMONITOR hmon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(mi)};
	GetMonitorInfoW(hmon, &mi);
	int left = mi.rcMonitor.left;
	int right = mi.rcMonitor.right;
	int top = mi.rcMonitor.top;
	int bottom = mi.rcMonitor.bottom;

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	switch (resolution)
	{
	case Window::Resolution::R1920X1080:
		width = 1920;
		height = 1080;
		break;
	case Window::Resolution::R1280X720:
		width = 1280;
		height = 720;
		break;
	case Window::Resolution::R640X480:
		width = 640;
		height = 480;
		break;
	case Window::Resolution::FULLSCREEN:
		width = 1936;
		height = 1096;
		break;
	default:
		assert(false && "Window::Window bad resolution");
		break;
	}

	//assert(width <= desktop.right && height <= desktop.bottom && width > 0 && height > 0 && "incorrect resolution");

	RECT rect;
	//rect.left = (desktop.right - width) / 2;
	//rect.top = (desktop.bottom - height) / 2;
	rect.left = 0;
	rect.top = 0;
	rect.right  = width  + rect.left;
	rect.bottom = height + rect.top;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	ASSERT_LAST_ERROR();

	hWnd = CreateWindowEx(
		0, WindowClass::GetName(),
		L"Practice Engine Window",
		WS_OVERLAPPEDWINDOW,
		rect.left, rect.top, width, height,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	m_hWindow = hWnd;

	ASSERT_LAST_ERROR();

	// set window api information to be able to use member functions
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	// make window visable
	ShowWindow(hWnd, SW_SHOW);


	// create directx graphics object
	pGraphics = std::make_unique<Graphics>(hWnd, width, height);
	
	// init imgui for win32
	ImGui_ImplWin32_Init(hWnd);

	if (Window::Resolution::FULLSCREEN == resolution)
		SetFullscreen(true);
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
	return *pGraphics;
}

Window::Resolution Window::GetResolution() const
{
	return resolution;
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
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		int x = mmi->ptMaxSize.x;
		int y = mmi->ptMaxSize.y;
		int xx = mmi->ptMaxPosition.x;
		int yy = mmi->ptMaxPosition.y;
		int k = 2;
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

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



