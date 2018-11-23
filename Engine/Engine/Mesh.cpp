#include "Mesh.h"



Mesh::Mesh()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	vertexBufferStride_ = sizeof(VertexPosTexNorm);
}


Mesh::~Mesh()
{
}

int Mesh::GetIndexCount()
{
	return m_indexCount;
}

void Mesh::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void Mesh::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	// Set vertex buffer stride and offset.
	stride = vertexBufferStride_;
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Mesh::CreateBuffers(ID3D11Device * device, void * vertices, int vertexCount, int bufferStride, Indice * indices, unsigned int indexCount)
{
	m_vertexCount = vertexCount;
	vertexBufferStride_ = bufferStride;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// Set up the description of the static vertex buffer.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = bufferStride * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	bool flag = SUCCEEDED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

	

	if (!indices)
	{
		m_indexCount = m_vertexCount;
		indices = new Indice[m_indexCount];
		if (!indices)
		{
			return false;
		}

		// Load the vertex array and index array with data.
		for (int i = 0; i< m_indexCount; ++i)
		{
			indices[i] = i;
		}
	}
	
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(Indice) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	return SUCCEEDED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)) && flag;
}
