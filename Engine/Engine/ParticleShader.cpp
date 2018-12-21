#include "ParticleShader.h"
#include "Particle.h"

ParticleShader::ParticleShader()
{
}


ParticleShader::~ParticleShader()
{
}

bool ParticleShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"../Engine/particle_vs.hlsl", L"../Engine/particle_ps.hlsl");
}

void ParticleShader::Shutdown()
{
	Shader::Shutdown();
}

bool ParticleShader::CreateInputLayout(ID3D11Device * device, ID3D10Blob * vertexShaderBlob)
{
	//Overrride the default input layout with the new one containing instancing
	return SUCCEEDED(device->CreateInputLayout(VertexInstancedPosTexNorm::getInputElements(), VertexInstancedPosTexNorm::getInputElementCount(), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		&m_layout));
}

bool ParticleShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, ID3D11ShaderResourceView* texture, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection)
{
	deviceContext->PSSetShaderResources(0, 1, &texture);
	return Shader::SetBaseParameters(deviceContext, mesh, world, view, projection);
}

void ParticleShader::DrawMesh(ID3D11DeviceContext * context, Mesh* mesh)
{
	//Override the default draw mesh to draw instances rather than indexed
	context->DrawInstanced(mesh->getVertexCount(), ((Particle*)mesh)->getInstanceCount(), 0, 0);
}