////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"
const D3DXVECTOR4 ambientLight(0.25f, 0.25f, 0.25f, 1.0f);

LightShaderClass::LightShaderClass()
{
	m_lightBuffer = 0;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}


LightShaderClass::~LightShaderClass()
{
}


bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	m_lightBuffer = D3DGraphics::CreateConstantBuffer(device, sizeof(LightBufferType));
	if (!m_lightBuffer)
	{
		return false;
	}
	return InitializeShader(device, hwnd, L"../Engine/light_vs.hlsl", L"../Engine/light_ps.hlsl");
}


void LightShaderClass::Shutdown()
{
	//Shutdown base
	Shader::Shutdown();

	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, Mesh* mesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
										   D3DXMATRIX projectionMatrix, D3DXVECTOR3 lightDirection, 
										   D3DXVECTOR4 diffuseColor, ID3D11ShaderResourceView* texture, float deltavalue)
{
	

	deviceContext->PSSetShaderResources(0, 1, &texture);
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	LightBufferType* dataPtr2;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientLight = ambientLight;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	bool flag = SetBaseParameters(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix);
	if (!flag)
	{
		return false;
	}
	return true;
}