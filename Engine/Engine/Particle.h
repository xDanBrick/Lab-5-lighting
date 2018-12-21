#pragma once
#include "Mesh.h"
#include "ComputeShader.h"

struct InstanceType
{
	D3DXVECTOR3 position;
};

class Particle : public Mesh
{
	public:
		Particle();
		~Particle();
		void					Shutdown() override;
		void					Render(ID3D11DeviceContext* deviceContext) override;
		bool					Initalise(ID3D11Device* device, HWND hwnd);
		inline int				getInstanceCount() { return m_InstanceCount; };
private:
	ID3D11Buffer *				m_InstanceBuffer;
	int							m_InstanceCount;
	InstanceType*				m_InstanceType;
	ComputeShader				m_ComputeShader;
	ID3D11Buffer*				m_InputBuffer, *m_OutputBuffer;
	ID3D11ShaderResourceView*	m_InputView;
	ID3D11UnorderedAccessView*	m_OutputUAV;
};

