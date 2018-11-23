////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYSHADERCLASS_H_
#define _SKYSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Shader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SkyShader
////////////////////////////////////////////////////////////////////////////////
class SkyShader : public Shader
{
private:
	struct SkyBufferType
	{
		D3DXVECTOR4 apexColor;
		D3DXVECTOR4 centreColor;
	};

public:
	SkyShader();
	SkyShader(const SkyShader&);
	~SkyShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, Mesh* mesh, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);
private:
	ID3D11Buffer* skyBuffer_;
};

#endif