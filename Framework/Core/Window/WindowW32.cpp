#include "Core/Window/Window.h"
#include "Core/PF_MinWin.h"


//==============================================================================================
// PLATFORM SPECIFIC -- Win32 internals
//==============================================================================================

namespace internal
{
	// Steal this from Windowsx.h
	#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
	#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))


	// Convert the public enum to the platform native enum
	DWORD ToPFEnum(WindowFlags flags)
	{
		// This actually seems to get optimized straight into a constant
		DWORD nativeFlag = 0;

		if (flags & WindowFlags::Normal)		nativeFlag |= WS_OVERLAPPEDWINDOW;
		if (flags & WindowFlags::FixedSize)		nativeFlag |= (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME);
		if (flags & WindowFlags::Borderless)	nativeFlag |= (WS_POPUP | WS_VISIBLE);
		if (flags & WindowFlags::FullScreen)	nativeFlag |= (WS_POPUP | WS_EX_TOPMOST);
		if (flags & WindowFlags::LockCursor)	nativeFlag |= 1 << 0;
		if (flags & WindowFlags::ForceMovable)	nativeFlag |= 1 << 1;

		return nativeFlag;
	}

	DWORD ExtractW32Style(DWORD flags)
	{
		return static_cast<DWORD>(flags) & 0x03; // 0011b
	}


	// Native underlying platform data
	struct WindowPF
	{
		// Native platform handles
		HINSTANCE	hinst;
		HWND		handle ;

		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
	
	WindowPF* pf = nullptr;
}

LRESULT CALLBACK internal::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	
	// Window has yet to be created
	if (window == nullptr)
	{
		// Calling DefWindowProc() is a must, CreateWindow relies on this to complete!
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	switch (msg)
	{
		case WM_CREATE:
			window->OnCreate();
			break;

		case WM_CLOSE:
			window->OnClose();
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			window->OnDestroy();
			PostQuitMessage(0);
			break;


		case WM_CHAR:
		{
			int8 virtualKey = static_cast<int8>(wParam);
			window->OnText(virtualKey);
			break;
		}

		case WM_KEYDOWN:
		{
			int8 virtualKey = static_cast<int8>(wParam);
			window->OnKeyPress(virtualKey, true);
			break;
		}

		case WM_KEYUP:
		{
			int8 virtualKey = static_cast<int8>(wParam);
			window->OnKeyPress(virtualKey, false);
			break;
		}

		case WM_MOUSEMOVE:
		{
			int32 mouseXAbsolute = GET_X_LPARAM(lParam);
			int32 mouseYAbsolute = GET_Y_LPARAM(lParam);
			window->OnMouseMove(mouseXAbsolute, mouseYAbsolute);
			break;
		}

		case WM_INPUT:
			// Not implemented yet, just pass dummy data...
			// window->OnKeyPress(0, false);
			// window->OnMouseMove(0, 0, 0, 0);
			break;

		case WM_SIZE:
		{
			int32 width  = LOWORD(lParam);
			int32 height = HIWORD(lParam);
			window->OnResize(width, height);
			break;
		}

		case WM_ACTIVATE:
		{
			bool isFocused = LOWORD(wParam) != 0;
			window->OnFocus(isFocused);
			break;
		}

		default:
			return DefWindowProc(pf->handle, msg, wParam, lParam);
	}

	return 0;
}

using namespace internal;

//==============================================================================================
// Window setup and destruction
//==============================================================================================

// Creates a native system window
bool Window::Create(StringView title, int32 width, int32 height, WindowFlags flags)
{
	InitPFStorage(WindowPF, pf_Storage);


	this->title = title.GetDataPtr();

	WNDCLASSEX wc		= { 0 };	
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= pf->hinst = GetModuleHandle(nullptr);
	wc.lpszClassName	= title.GetDataPtr();
	wc.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);

	if (RegisterClassEx(&wc) == 0)
	{
		// Can't say we're off to a good start if we've blown up already...
		return false;
	}
	
	// Need to compensate for the window border if it exists, otherwise the inner size will not be correct
	int32	scaledWidth		= width;
	int32	scaledHeight	= height;
	RECT	rect			= { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	const DWORD style		= ToPFEnum(flags);

	if (flags != WindowFlags::Borderless)
	{
		AdjustWindowRect(&rect, style, false);
		scaledWidth		= (rect.right - rect.left);
		scaledHeight	= (rect.bottom - rect.top);	
	}

	// Push the top left point so the window falls in the center of the screen
	const int32 centeredX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (scaledWidth  / 2);
	const int32 centeredY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (scaledHeight / 2);


	// WndProc is called DURING this - it MUST do some setup if this is to succeed
	pf->handle = CreateWindowEx(0, title.GetDataPtr(), title.GetDataPtr(), style, centeredX, centeredY, scaledWidth, scaledHeight, 0, 0, 0, 0);

	if (pf->handle)
	{
		// Save the this pointer so we can access class data later
		SetWindowLongPtr(pf->handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		// We should be ok at this point, show the window
		::ShowWindow(pf->handle, SW_SHOW);
		return true;
	}

	return false;
}

// Gracefully destroys a window
bool Window::Destroy()
{
	return ((uintptr_t)GetModuleHandle(nullptr) > 0) ? true : false;
}


//==============================================================================================
// Windows message pump and events
//==============================================================================================

// Gets an event from the queue, but returns immediately if there is nothing
bool Window::GetEvents() const
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.message == WM_QUIT) ? false : true;
}

// Gets an event from the queue, but will wait if there is nothing
bool Window::GetEventsWait() const
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.message == WM_QUIT) ? false : true;
}


//==============================================================================================
// Windows state management and helpers
//==============================================================================================

// Sets and updates the window title
void Window::SetTitle(StringView title)
{
	this->title = Move(title.GetDataPtr());
	SetWindowText(pf->handle, title.GetDataPtr());
}

// Sets and updates the window flags
void Window::SetFlags(WindowFlags flags)
{
	const DWORD w32Flags	= ToPFEnum(flags);
	const DWORD style		= ExtractW32Style(w32Flags);

	SetWindowLongPtr(pf->handle, GWL_STYLE, style);

	// TODO: Implement our own flags
}

// Sets and updates the window position
void Window::SetPosition(int32 x, int32 y)
{
	SetWindowPos(pf->handle, nullptr, x, y, w_width, w_width, 0);
}

// Sets and updates the window dimensions
void Window::SetDimensions(int32 width, int32 height)
{
	w_width		= width;
	w_height	= height;

	SetWindowPos(pf->handle, nullptr, 0, 0, w_width, w_width, 0);
}


// Controls cursor visibility
void Window::ShowCursor(bool isVisible)
{
	::ShowCursor(isVisible);
}

// Controls window visibility
void Window::ShowWindow(bool isVisible)
{
	::ShowWindow(pf->handle, isVisible ? SW_SHOW : SW_HIDE);
}


// Tests if the window is currently minimized
bool Window::IsMinimized() const
{
	return IsIconic(pf->handle) == NULL;
}

// Tests if the window is currently maximized
bool Window::IsMaximized() const
{
	return IsIconic(pf->handle) != NULL;
}

// Tests if the window is valid and created
bool Window::IsValid() const
{
	return false;
}


// Returns the current window title
String Window::GetTitle() const
{
	return Move(title);
}


void* Window::GetHandle() const
{
	return pf->handle;
}
