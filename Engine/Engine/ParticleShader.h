#pragma once
#include "Shader.h"


class ParticleShader : public Shader
{
public:
	ParticleShader();
	~ParticleShader();

	bool			Initialize(ID3D11Device*, HWND) override;
	void			Shutdown() override;
	bool			CreateInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob) override;

	bool			Render(ID3D11DeviceContext*, Mesh* mesh, ID3D11ShaderResourceView* texture, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void			DrawMesh(ID3D11DeviceContext * context, Mesh* mesh) override;
private:
	
};

