#pragma once
#include "D3DGraphics.h"

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();
	bool Initalise(ID3D11Device* device, HWND hwnd, const wchar_t* shaderPath);
	void Dispatch(ID3D11DeviceContext* deviceContext, int threadGroupX = 1, int threadGroupY = 1, int threadGroupZ = 1);
private:
	ID3D11ComputeShader * computeShader_;
};

