#include "Window.h"
#include <assert.h>
#include <sstream>
#include "MessageMap.h"
#include "Assert.h"
#include "resource.h"

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
	wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
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

Window::Window(int width, int height, const wchar_t * name)
	:
	width(width),
	height(height),
	mouse(width, height)
{
	// get the screen resolution to center the window
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	assert(width <= desktop.right && height <= desktop.bottom && width > 0 && height && "incorrect resolution");

	RECT rect;
	rect.left = (desktop.right  - width ) / 2;
	rect.top  = (desktop.bottom - height) / 2;
	rect.right  = width  + rect.left;
	rect.bottom = height + rect.top;

	AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX| WS_SYSMENU, FALSE);

	ASSERT_LAST_ERROR();

	HWND hWnd = CreateWindowEx(
		0, WindowClass::GetName(),
		L"Practice Engine Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		rect.left, rect.top, width, height,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	ASSERT_LAST_ERROR();

	// set window api information to be able to use member functions
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	// make window visable
	ShowWindow(hWnd, SW_SHOW);

	// create directx graphics object
	pGraphics = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
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

LRESULT Window::HandleThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->Handle(hWnd, msg, wParam, lParam);
}

LRESULT Window::Handle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

