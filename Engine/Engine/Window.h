#pragma once
#include <Windows.h>

class SubWindow
{
public:
	SubWindow();
	virtual								~SubWindow();
	inline void							Show(int flag) { ShowWindow(hwnd_, flag); };
	inline HWND							getHWND() { return hwnd_; };
	int									getId() { return id_; };
	inline HWND							getParent() { return GetParent(hwnd_); };
	bool								Initalise(LPCWSTR className, LPCWSTR windowName, int arguments, int x, int y, int width, int height, HWND parent, int id);
	protected:
		HWND							hwnd_;
		
		int								id_;
};

class Window : public SubWindow
{
	public:
		Window();
		~Window();
		bool						Initalise(WNDPROC proc, LPCWSTR className, LPCWSTR windowName, int arguments, int x, int y, int width, int height, HWND parent = nullptr);
		inline void					setForeground(){
			SetForegroundWindow(hwnd_);
			SetFocus(hwnd_);
		};
private:
	LPCWSTR							className_;
};

static HFONT CreateWindowFont(const wchar_t* fontName, int size, int weight)
{
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = size;
	logFont.lfWeight = weight;
	lstrcpyW(logFont.lfFaceName, fontName);
	return CreateFontIndirect(&logFont);
}
