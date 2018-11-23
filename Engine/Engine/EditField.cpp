#include "EditField.h"
#include <string>
#include <sstream>

EditField::EditField()
{
}


EditField::~EditField()
{
}

bool EditField::Initalise(float * value, int x, int y, int width, int height, HWND parent, int id)
{
	valuePtr_ = value;
	return SubWindow::Initalise(L"EDIT", std::to_wstring(*value).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, width, height, parent, id);
}

void EditField::OnCommand(WPARAM wParam)
{
	

	if (LOWORD(wParam) == getId())
	{
		if (HIWORD(wParam) == EN_CHANGE)
		{
			char buff[100];
			GetWindowTextA(hwnd_, buff, sizeof(buff));
			std::string text = buff;

			float value = *valuePtr_;
			if (text.empty())
			{
				*valuePtr_ = 0.0f;
				return;
			}
			std::istringstream iss(text);
			iss >> std::noskipws >> value;
			if (iss && iss.eof())
			{
				*valuePtr_ = value;
				return;
			}
			
		}
	}
}
