#include "CloudShader.h"

CloudShader::CloudShader()
{
	for (int i = 0; i < 2; ++i)
	{
		translations_[i] = 0.0f;
	}
}


CloudShader::~CloudShader()
{
}

bool CloudShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	bool flag = cloudBuffer_.Initalise(device);
	if (!flag)
	{
		return false;
	}
	return InitializeShader(device, hwnd, L"../Engine/cloud_vs.hlsl", L"../Engine/cloud_ps.hlsl");
}

void CloudShader::Shutdown()
{
	Shader::Shutdown();
}

bool CloudShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView * cloud, ID3D11ShaderResourceView * noise, D3DXVECTOR2 scales, D3DXVECTOR2 brightness, D3DXVECTOR2 speed)
{
	for (int i = 0; i < 2; ++i)
	{
		translations_[i] += speed[i];
		if (translations_[i] > 1.0f)
		{
			translations_[i] -= 1.0f;
		}
		cloudBuffer_.getValuePtr()->translation[i] = translations_[i];
		cloudBuffer_.getValuePtr()->scale[i] = scales[i];
		cloudBuffer_.getValuePtr()->brightness[i] = brightness[i];
		cloudBuffer_.getValuePtr()->padding[i] = 0.0f;
	}
	
	cloudBuffer_.Map(deviceContext);

	auto cloudBuffer = cloudBuffer_.getBuffer();
	deviceContext->PSSetConstantBuffers(0, 1, &cloudBuffer);
	deviceContext->PSSetShaderResources(0, 1, &cloud);
	deviceContext->PSSetShaderResources(1, 1, &noise);
	return Shader::SetBaseParameters(deviceContext, mesh, world, view, projection);
}
