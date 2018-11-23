#pragma once
#include "Shader.h"

class PBRShader : public Shader
{
	public:
		PBRShader();
		~PBRShader();

		bool			Initialize(ID3D11Device*, HWND) override;
		bool			Render(ID3D11DeviceContext * context, Mesh * mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj, D3DXVECTOR3 albedoColor, float shininess, float smoothness);
		void			Shutdown() override;

		struct MaterialBuffer
		{
			D3DXVECTOR3 albedoColor;
			float smoothness;
			float shininess;
			D3DXVECTOR3 padding;
		};
		ConstantBuffer<MaterialBuffer> materialBuffer_;
};

