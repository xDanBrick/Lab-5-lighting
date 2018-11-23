#include "VertexTypes.h"

D3D11_INPUT_ELEMENT_DESC posTexNormal[3];
D3D11_INPUT_ELEMENT_DESC instancedPosTexNormal[4];

VertexPosTexNorm::VertexPosTexNorm(D3DXVECTOR3 position, D3DXVECTOR2 texture, D3DXVECTOR3 normal)
{
	this->position = position;
	this->texture = texture;
	this->normal = normal;
}

D3D11_INPUT_ELEMENT_DESC* VertexPosTexNorm::getInputElements()
{	
	if (posTexNormal[0].SemanticName != "POSITION")
	{
		posTexNormal[0].SemanticName = "POSITION";
		posTexNormal[0].SemanticIndex = 0;
		posTexNormal[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		posTexNormal[0].InputSlot = 0;
		posTexNormal[0].AlignedByteOffset = 0;
		posTexNormal[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		posTexNormal[0].InstanceDataStepRate = 0;

		posTexNormal[1].SemanticName = "TEXCOORD";
		posTexNormal[1].SemanticIndex = 0;
		posTexNormal[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		posTexNormal[1].InputSlot = 0;
		posTexNormal[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		posTexNormal[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		posTexNormal[1].InstanceDataStepRate = 0;

		posTexNormal[2].SemanticName = "NORMAL";
		posTexNormal[2].SemanticIndex = 0;
		posTexNormal[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		posTexNormal[2].InputSlot = 0;
		posTexNormal[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		posTexNormal[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		posTexNormal[2].InstanceDataStepRate = 0;
	}
	
	return posTexNormal;
}

unsigned int VertexPosTexNorm::getInputElementCount()
{
	return 3;
}

D3D11_INPUT_ELEMENT_DESC* VertexInstancedPosTexNorm::getInputElements()
{
	if (instancedPosTexNormal[0].SemanticName != "POSITION")
	{
		instancedPosTexNormal[0].SemanticName = "POSITION";
		instancedPosTexNormal[0].SemanticIndex = 0;
		instancedPosTexNormal[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		instancedPosTexNormal[0].InputSlot = 0;
		instancedPosTexNormal[0].AlignedByteOffset = 0;
		instancedPosTexNormal[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		instancedPosTexNormal[0].InstanceDataStepRate = 0;

		instancedPosTexNormal[1].SemanticName = "TEXCOORD";
		instancedPosTexNormal[1].SemanticIndex = 0;
		instancedPosTexNormal[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		instancedPosTexNormal[1].InputSlot = 0;
		instancedPosTexNormal[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		instancedPosTexNormal[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		instancedPosTexNormal[1].InstanceDataStepRate = 0;

		instancedPosTexNormal[2].SemanticName = "NORMAL";
		instancedPosTexNormal[2].SemanticIndex = 0;
		instancedPosTexNormal[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		instancedPosTexNormal[2].InputSlot = 0;
		instancedPosTexNormal[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		instancedPosTexNormal[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		instancedPosTexNormal[2].InstanceDataStepRate = 0;

		instancedPosTexNormal[3].SemanticName = "TEXCOORD";
		instancedPosTexNormal[3].SemanticIndex = 1;
		instancedPosTexNormal[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		instancedPosTexNormal[3].InputSlot = 1;
		instancedPosTexNormal[3].AlignedByteOffset = 0;
		instancedPosTexNormal[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		instancedPosTexNormal[3].InstanceDataStepRate = 1;
	}

	return instancedPosTexNormal;
}

unsigned int VertexInstancedPosTexNorm::getInputElementCount()
{
	return 4;
}
