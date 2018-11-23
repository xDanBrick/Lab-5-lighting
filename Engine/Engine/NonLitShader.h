#pragma once
#include "Shader.h"

class NonLitShader : public Shader
{
	public:
		NonLitShader();
		~NonLitShader();

		bool			Initialize(ID3D11Device*, HWND) override;
		bool			Render(ID3D11DeviceContext * context, Mesh * mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj, ID3D11ShaderResourceView* resourceView);
		void			Shutdown() override;

		/*struct MaterialBuffer
		{
			D3DXVECTOR3 albedoColor;
			float smoothness;
			float shininess;
			D3DXVECTOR3 padding;
		};
		ConstantBuffer<MaterialBuffer> materialBuffer_;*/
};

