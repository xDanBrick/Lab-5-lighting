#pragma once
#include "Window.h"

class EditField : public SubWindow
{
public:
	EditField();
	~EditField();
	bool				Initalise(float* value, int x, int y, int width, int height, HWND parent, int id);

	void				OnCommand(WPARAM wparam);
private:
	float*				valuePtr_;
};

