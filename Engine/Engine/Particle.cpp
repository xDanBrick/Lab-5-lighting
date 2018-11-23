#include "Particle.h"
#include <vector>

#define PARTICLE_COUNT 1000

Particle::Particle()
{
	instanceCount_ = 1000;
	instanceType_ = new InstanceType[PARTICLE_COUNT];
}

Particle::~Particle()
{
}

void Particle::Shutdown()
{
}

void Particle::Render(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto hr = deviceContext->Map(inputBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		InstanceType* dataView = reinterpret_cast<InstanceType*>(mappedResource.pData);
		for (int i = 0; i < PARTICLE_COUNT; ++i)
		{
			dataView[i].position = instanceType_[i].position;
		}
		deviceContext->Unmap(inputBuffer_, 0);
	}

	ID3D11Device* device;
	deviceContext->GetDevice(&device);
	deviceContext->CSSetShaderResources(0, 1, &inputView_);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &outputUAV_, 0);

	computeShader_.Dispatch(deviceContext, 10);

	ID3D11ShaderResourceView* view = nullptr;

	deviceContext->CSSetShaderResources(0, 1, &view);

	deviceContext->CopyResource(m_instanceBuffer, outputBuffer_);
	//deviceContext->CopyResource(outputResultBuffer_, outputBuffer_);

	// Update particle system data with output from Compute Shader
	hr = deviceContext->Map(outputBuffer_, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		auto dataView = (InstanceType*)mappedResource.pData;

		for (int i = 0; i < PARTICLE_COUNT; ++i)
		{
			//(InstanceType*)outputBuffer_[i];
			instanceType_[i].position = dataView[i].position;
		}
		UpdateInstanceBuffer(device, dataView, PARTICLE_COUNT);

		deviceContext->Unmap(outputBuffer_, 0);
	}

	// Set vertex buffer stride and offset.
	unsigned int stride[2] = { vertexBufferStride_, sizeof(InstanceType) };
	unsigned int offset[2] = { 0, 0 };
	ID3D11Buffer* buffers[2] = { m_vertexBuffer, m_instanceBuffer };
	
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Particle::UpdateInstanceBuffer(ID3D11Device* device, InstanceType* positions, int count)
{
	//instanceType_ = positions;
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * count;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;
	
	// Give the subresource structure a pointer to the instance data.
	
	instanceData.pSysMem = instanceType_;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}

	instanceCount_ = count;
	
	// Release the instance array now that the instance buffer has been created and loaded.
	return true;
}

bool Particle::Initalise(ID3D11Device * device, HWND hwnd)
{
	std::vector<VertexPosTexNorm> vertices;

	float hSize = 0.5f;
	
	vertices.push_back({ { hSize, hSize, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, -hSize, 0.0f },  { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, -hSize, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });

	vertices.push_back({ { -hSize, -hSize, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, hSize, 0.0f },  { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, hSize, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } });

	if (!computeShader_.Initalise(device, hwnd, L"../Engine/ParticleCompute.hlsl"))
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////
	//					Input
	HRESULT hr;

	D3D11_BUFFER_DESC constantDataDesc;
	constantDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantDataDesc.ByteWidth = sizeof(InstanceType) * PARTICLE_COUNT;
	constantDataDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constantDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantDataDesc.StructureByteStride = sizeof(InstanceType);
	constantDataDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = device->CreateBuffer(&constantDataDesc, 0, &inputBuffer_);

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		instanceType_[i].position = D3DXVECTOR3((float)(rand() % 500 - 250), (float)(rand() % 100), (float)(rand() % 500 - 250));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = instanceCount_;

	hr = device->CreateShaderResourceView(inputBuffer_, &srvDesc, &inputView_);

	/////////////////////////////////////////////////////////////////////////
	//							Output
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(InstanceType) * instanceCount_;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	outputDesc.StructureByteStride = sizeof(InstanceType);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = device->CreateBuffer(&outputDesc, 0, &outputBuffer_);

	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = device->CreateBuffer(&outputDesc, 0, &outputResultBuffer_);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = instanceCount_;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	hr = device->CreateUnorderedAccessView(outputBuffer_, &uavDesc, &outputUAV_);

	D3D11_BUFFER_DESC instanceBufferDesc;
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * PARTICLE_COUNT;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Create the instance buffer.
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, nullptr, &m_instanceBuffer)))
	{
		return false;
	}

	return CreateBuffers(device, vertices.data(), vertices.size());
}
