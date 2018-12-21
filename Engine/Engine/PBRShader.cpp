#include "PBRShader.h"



PBRShader::PBRShader()
{
}


PBRShader::~PBRShader()
{
}

bool PBRShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	//Initalise the material buffer
	if (!materialBuffer_.Initalise(device))
	{
		return false;
	}
	materialBuffer_.getValuePtr()->padding = { 0.0f, 0.0f, 0.0f };
	return Shader::InitializeShader(device, hwnd, L"../Engine/pbr_vs.hlsl", L"../Engine/pbr_ps.hlsl");
}

bool PBRShader::Render(ID3D11DeviceContext* deviceContext, Mesh* mesh, ID3D11ShaderResourceView* texture, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR3 albedoColor, float shininess, float smoothness)
{
	//Update the material buffer
	materialBuffer_.getValuePtr()->albedoColor = albedoColor;
	materialBuffer_.getValuePtr()->shininess = shininess;
	materialBuffer_.getValuePtr()->smoothness = smoothness;
	materialBuffer_.Map(deviceContext);

	auto buffer = materialBuffer_.getBuffer();
	deviceContext->PSSetConstantBuffers(0, 1, &buffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return SetBaseParameters(deviceContext, mesh, worldMatrix, viewMatrix, projectionMatrix);
}

void PBRShader::Shutdown()
{
	Shader::Shutdown();
}

