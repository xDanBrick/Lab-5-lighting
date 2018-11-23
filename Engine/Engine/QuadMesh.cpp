#include "QuadMesh.h"
#include <vector>

QuadMesh::QuadMesh(ID3D11Device* device)
{
	std::vector<VertexPosTexNorm> vertices;

	float hSize = 0.5f;

	vertices.push_back({ { -hSize, 0.0f, -hSize },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, 0.0f, -hSize },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hSize, 0.0f, hSize },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });

	vertices.push_back({ { hSize, 0.0f, hSize },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, 0.0f, hSize },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { -hSize, 0.0f, -hSize },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	CreateBuffers(device, vertices.data(), vertices.size());
}

QuadMesh::~QuadMesh()
{
}
