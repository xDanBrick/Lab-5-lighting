////////////////////////////////////////////////////////////////////////////////
// Filename: SkyShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SkyShader.h"

SkyShader::SkyShader()
{
	skyBuffer_ = nullptr;
}


SkyShader::SkyShader(const SkyShader& other)
{
}


SkyShader::~SkyShader()
{
}


bool SkyShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	skyBuffer_ = D3DGraphics::CreateConstantBuffer(device, sizeof(SkyBufferType));
	if (!skyBuffer_)
	{
		return false;
	}
	return InitializeShader(device, hwnd, L"../Engine/sky_vs.hlsl", L"../Engine/sky_ps.hlsl");
}


void SkyShader::Shutdown()
{
	//Shutdown base
	Shader::Shutdown();

	// Release the light constant buffer.
	if (skyBuffer_)
	{
		skyBuffer_->Release();
		skyBuffer_ = 0;
	}

	return;
}

bool SkyShader::Render(ID3D11DeviceContext* deviceContext, Mesh* mesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
										   D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, 
										   D3DXVECTOR4 centreColor)
{
	

	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	SkyBufferType* dataPtr2;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(skyBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (SkyBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->apexColor = apexColor;
	dataPtr2->centreColor = centreColor;

	// Unlock the constant buffer.
	deviceContext->Unmap(skyBuffer_, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &skyBuffer_);

	return SetBaseParameters(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix);
}