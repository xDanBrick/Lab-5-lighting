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
		void			Shutdown() override;
		void			Render(ID3D11DeviceContext* deviceContext) override;
		bool			UpdateInstanceBuffer(ID3D11Device* device, InstanceType* positions, int count);
		bool			Initalise(ID3D11Device* device, HWND hwnd);
		inline int		getInstanceCount() { return instanceCount_; };
private:
	ID3D11Buffer *		m_instanceBuffer;
	int					instanceCount_;
	InstanceType*		instanceType_;
	ComputeShader	computeShader_;
	ID3D11Buffer*	inputBuffer_, *outputBuffer_, *outputResultBuffer_;
	ID3D11ShaderResourceView* inputView_;
	ID3D11UnorderedAccessView* outputUAV_;
};

