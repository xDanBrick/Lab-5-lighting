////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADERCLASS_H_
#define _SHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "D3DGraphics.h"
#include "Mesh.h"

class Shader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct VariableBufferType
	{
		float delta;
		D3DXVECTOR3 padding;
	};

public:
							Shader();
							Shader(const Shader&);
	virtual					~Shader();

	virtual bool			Initialize(ID3D11Device*, HWND) { return true; };
	virtual void			Shutdown();
	bool					Render(ID3D11DeviceContext*, Mesh* mesh);


protected:
	bool					InitializeShader(ID3D11Device*, HWND, const wchar_t*, const wchar_t*);
	bool					SetBaseParameters(ID3D11DeviceContext*, Mesh* mesh, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	virtual bool			CreateInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBlob);
	ID3D11InputLayout*		m_layout;
	virtual void			DrawMesh(ID3D11DeviceContext* context, Mesh* mesh);
	ID3D11SamplerState*		m_sampleState;
private:
private:
	ID3D11VertexShader*		m_vertexShader;
	ID3D11PixelShader*		m_pixelShader;

	ConstantBuffer<MatrixBufferType>	matrixBuffer_;
};

#endif