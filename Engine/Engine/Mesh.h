#pragma once
#include "VertexTypes.h"
using namespace std;

typedef unsigned long Indice;

class Mesh
{
	public:
		Mesh();
		~Mesh();
		int				GetIndexCount();
		inline int		getVertexCount() { return m_vertexCount; };
		virtual void	Shutdown();
		virtual void	Render(ID3D11DeviceContext* deviceContext);
protected:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	bool				CreateBuffers(ID3D11Device* device, void* vertices, int count, int bufferStride = sizeof(VertexPosTexNorm), Indice* indices = nullptr, unsigned int indexCount = 0);
	ID3D11Buffer*	m_vertexBuffer, *m_indexBuffer;
	int				vertexBufferStride_;
private:
	
	int				m_vertexCount, m_indexCount;
};

