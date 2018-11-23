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
	return SUCCEEDED(device->CreateInputLayout(VertexInstancedPosTexNorm::getInputElements(), VertexInstancedPosTexNorm::getInputElementCount(), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		&m_layout));
}

bool ParticleShader::Render(ID3D11DeviceContext * context, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection)
{
	return Shader::SetBaseParameters(context, mesh, world, view, projection);
}

void ParticleShader::DrawMesh(ID3D11DeviceContext * context, Mesh* mesh)
{
	context->DrawInstanced(mesh->getVertexCount(), ((Particle*)mesh)->getInstanceCount(), 0, 0);
}