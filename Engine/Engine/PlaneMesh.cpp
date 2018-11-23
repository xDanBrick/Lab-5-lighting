#include "PlaneMesh.h"
#include <vector>

void CreatePlane(std::vector<VertexPosTexNorm>& vertices, float size, int tesselation, int textureRepeat)
{
	float u, v, increment;
	// UV coords.
	u = 0;
	v = 0;

	float posInc = size / tesselation;

	increment = (1.0f / tesselation) * (float)textureRepeat;
	float hSize = (float)size * 0.5f;
	// Load the vertex and index arrays with the terrain data.
	for (float j = -hSize; j < hSize; j += posInc)
	{
		for (float i = -hSize; i < hSize; i += posInc)
		{
			vertices.push_back({ { i, 0.0f, j + posInc },{ u, v + increment },{ 0.0f, 1.0f, 0.0f } });
			vertices.push_back({ { i + posInc, 0.0f, j + posInc },{ u + increment, v + increment },{ 0.0f, 1.0f, 0.0f } });
			vertices.push_back({ { i + posInc, 0.0f, j },{ u + increment, v },{ 0.0f, 1.0f, 0.0f } });
			
			vertices.push_back({ { i + posInc, 0.0f, j },{ u + increment, v },{ 0.0f, 1.0f, 0.0f } });
			vertices.push_back({ { i, 0.0f, j },{ u, v },{ 0.0f, 1.0f, 0.0f } });
			vertices.push_back({ { i, 0.0f, j + posInc },{ u, v + increment },{ 0.0f, 1.0f, 0.0f } });
			u += increment;
		}
		u = 0;
		v += increment;
	}
}

PlaneMesh::PlaneMesh(ID3D11Device* device)
{
	std::vector<VertexPosTexNorm> vertices;
	CreatePlane(vertices, 256.0f, 256, 1);

	CreateBuffers(device, vertices.data(), vertices.size());
}

PlaneMesh::~PlaneMesh()
{
}
