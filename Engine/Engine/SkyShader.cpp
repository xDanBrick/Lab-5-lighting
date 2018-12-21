////////////////////////////////////////////////////////////////////////////////
// Filename: SkyShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SkyShader.h"

SkyShader::SkyShader()
{
}


SkyShader::SkyShader(const SkyShader& other)
{
}


SkyShader::~SkyShader()
{
}


bool SkyShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	if (!skyBuffer_.Initalise(device))
	{
		return false;
	}
	return InitializeShader(device, hwnd, L"../Engine/sky_vs.hlsl", L"../Engine/sky_ps.hlsl");
}


void SkyShader::Shutdown()
{
	//Shutdown base
	Shader::Shutdown();
}

bool SkyShader::Render(ID3D11DeviceContext* deviceContext, Mesh* mesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
										   D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, 
										   D3DXVECTOR4 centreColor)
{
	// Copy the sky colors into the buffer
	skyBuffer_.getValuePtr()->apexColor = apexColor;
	skyBuffer_.getValuePtr()->centreColor = centreColor;
	skyBuffer_.Map(deviceContext);
	
	auto buffer = skyBuffer_.getBuffer();
	deviceContext->PSSetConstantBuffers(0, 1, &buffer);

	return SetBaseParameters(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix);
}