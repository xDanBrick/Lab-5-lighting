#include "Window.h"

Window::Window()
{

	className_ = nullptr;
}

Window::~Window()
{
	if (className_)
	{
		UnregisterClass(className_, GetModuleHandle(nullptr));
	}
}

bool Window::Initalise(WNDPROC proc, LPCWSTR className, LPCWSTR windowName, int arguments, int x, int y, int width, int height, HWND parent)
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(70, 130, 180));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);
	className_ = className;

	
	SubWindow::Initalise(className, windowName, arguments, x, y, width, height, parent, 0);
	
	return hwnd_;
}

SubWindow::SubWindow()
{
	hwnd_ = nullptr;
}

SubWindow::~SubWindow()
{
	if (hwnd_)
	{
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}
}

bool SubWindow::Initalise(LPCWSTR className, LPCWSTR windowName, int arguments, int x, int y, int width, int height, HWND parent, int id)
{
	hwnd_ = CreateWindow(className, windowName, arguments,
		x, y, width, height, parent, (HMENU)id, GetModuleHandle(nullptr), NULL);
	id_ = id;
	return hwnd_;
}
