#include "Particle.h"
#include <vector>

const int PARTICLE_COUNT = 1000;
const int BOUNDS_X = 500;
const int BOUNDS_Y = 100;

Particle::Particle()
{
	m_InstanceCount = 1000;
	m_InstanceType = new InstanceType[PARTICLE_COUNT];
}

Particle::~Particle()
{
	if (m_InstanceType)
	{
		delete[] m_InstanceType;
		m_InstanceType = nullptr;
	}
}

void Particle::Shutdown()
{
}

void Particle::Render(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//Map the inital values to the input buffer
	if (SUCCEEDED(deviceContext->Map(m_InputBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		InstanceType* dataView = reinterpret_cast<InstanceType*>(mappedResource.pData);
		for (int i = 0; i < PARTICLE_COUNT; ++i)
		{
			dataView[i].position = m_InstanceType[i].position;
		}
		deviceContext->Unmap(m_InputBuffer, 0);
	}

	//Run the compute shader with the input and output buffers
	deviceContext->CSSetShaderResources(0, 1, &m_InputView);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_OutputUAV, 0);
	m_ComputeShader.Dispatch(deviceContext, 10);

	deviceContext->CopyResource(m_InstanceBuffer, m_OutputBuffer);

	// Update the instances with the new instances from the compute shader
	if (SUCCEEDED(deviceContext->Map(m_OutputBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
	{
		auto dataView = (InstanceType*)mappedResource.pData;

		for (int i = 0; i < PARTICLE_COUNT; ++i)
		{
			m_InstanceType[i].position = dataView[i].position;
		}
		deviceContext->Unmap(m_OutputBuffer, 0);
	}

	// Set vertex buffer stride and offset.
	unsigned int stride[2] = { vertexBufferStride_, sizeof(InstanceType) };
	unsigned int offset[2] = { 0, 0 };
	ID3D11Buffer* buffers[2] = { m_vertexBuffer, m_InstanceBuffer };
	
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Particle::Initalise(ID3D11Device * device, HWND hwnd)
{
	//Set inital position for particles
	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		m_InstanceType[i].position = D3DXVECTOR3((float)((rand() % BOUNDS_X) - (BOUNDS_X / 2)), (float)(rand() % BOUNDS_Y), (float)((rand() % BOUNDS_X) - (BOUNDS_X / 2)));
	}

	//Initalise the compute shader
	if (!m_ComputeShader.Initalise(device, hwnd, L"../Engine/ParticleCompute.hlsl"))
	{
		return false;
	}

	//Create input buffer for compute shader
	m_InputBuffer = D3DGraphics::CreateBuffer(device, D3D11_USAGE_DYNAMIC, sizeof(InstanceType) * PARTICLE_COUNT, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, sizeof(InstanceType), D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

	//Create shader resource for input buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = m_InstanceCount;
	device->CreateShaderResourceView(m_InputBuffer, &srvDesc, &m_InputView);

	//Create output buffer for compute shader
	m_OutputBuffer = D3DGraphics::CreateBuffer(device, D3D11_USAGE_DEFAULT, sizeof(InstanceType) * m_InstanceCount, D3D11_BIND_UNORDERED_ACCESS, D3D11_CPU_ACCESS_READ, sizeof(InstanceType), D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

	//Create UAV in output buffer
	m_OutputUAV = D3DGraphics::CreateBufferUnorderedAccessView(device, m_OutputBuffer, m_InstanceCount);

	//Create instance buffer for the particles
	m_InstanceBuffer = D3DGraphics::CreateBuffer(device, D3D11_USAGE_DEFAULT, sizeof(InstanceType) * PARTICLE_COUNT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0);

	//Create initalise vertices
	std::vector<VertexPosTexNorm> vertices;

	float hSize = 0.5f;
	vertices.push_back({ { hSize, hSize, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, -hSize, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, -hSize, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });

	vertices.push_back({ { -hSize, -hSize, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, hSize, 0.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, hSize, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });

	//Create the vertex and index buffers
	return CreateBuffers(device, vertices.data(), vertices.size());
}
