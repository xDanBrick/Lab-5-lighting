#include "NonLitShader.h"



NonLitShader::NonLitShader()
{
}


NonLitShader::~NonLitShader()
{
}

bool NonLitShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	return Shader::InitializeShader(device, hwnd, L"../Engine/nonlit_vs.hlsl", L"../Engine/nonlit_ps.hlsl");
}

bool NonLitShader::Render(ID3D11DeviceContext* deviceContext, Mesh* mesh, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* resourceView)
{

	deviceContext->PSSetShaderResources(0, 1, &resourceView);

	return SetBaseParameters(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix);
}

void NonLitShader::Shutdown()
{
	Shader::Shutdown();
}

