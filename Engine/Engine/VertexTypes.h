#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

struct VertexPosTexNorm
{
	VertexPosTexNorm() {};
	VertexPosTexNorm(D3DXVECTOR3 position, D3DXVECTOR2 texture, D3DXVECTOR3 normal);
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;

	static D3D11_INPUT_ELEMENT_DESC*	getInputElements();
	static unsigned int					getInputElementCount();
};

struct VertexInstancedPosTexNorm : public VertexPosTexNorm
{
	static D3D11_INPUT_ELEMENT_DESC*	getInputElements();
	static unsigned int					getInputElementCount();
};
