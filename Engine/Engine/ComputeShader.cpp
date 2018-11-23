#include "ComputeShader.h"

ComputeShader::ComputeShader()
{
}


ComputeShader::~ComputeShader()
{
}

bool ComputeShader::Initalise(ID3D11Device * device, HWND hwnd, const wchar_t* shaderPath)
{
	if (ID3D10Blob* shaderBlob = D3DGraphics::CompileShader(device, hwnd, shaderPath, "cs_5_0"))
	{
		return SUCCEEDED(device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &computeShader_));
	}
	return false;
}

void ComputeShader::Dispatch(ID3D11DeviceContext* deviceContext, int threadGroupX, int threadGroupY, int threadGroupZ)
{
	deviceContext->CSSetShader(computeShader_, nullptr, 0);
	deviceContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
	deviceContext->CSSetShader(nullptr, nullptr, 0);
	UINT initCounts = 0;
	ID3D11UnorderedAccessView* view = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &view, &initCounts);
}
