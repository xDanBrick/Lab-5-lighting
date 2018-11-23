#pragma once
#include "Shader.h"

class CloudShader : public Shader
{
public:
	CloudShader();
	~CloudShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj, ID3D11ShaderResourceView* cloud, ID3D11ShaderResourceView* noise, D3DXVECTOR2 scales, D3DXVECTOR2 brightness, D3DXVECTOR2 speed);
private:
	struct CloudBuffer
	{
		D3DXVECTOR2 translation;
		D3DXVECTOR2 scale;
		D3DXVECTOR2 brightness;
		D3DXVECTOR2 padding;
	};
	ConstantBuffer<CloudBuffer> cloudBuffer_;
	D3DXVECTOR2 translations_;
};

