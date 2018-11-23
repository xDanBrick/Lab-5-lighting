#include "OrthoMesh.h"
#include <vector>

OrthoMesh::OrthoMesh()
{
	
}


OrthoMesh::~OrthoMesh()
{
}

bool OrthoMesh::Initalise(ID3D11Device * device, int width, int height)
{
	float hwidth = width;
	float hheight = height;

	float hleft, hright, htop, hbottom;
	hleft = (float)((hwidth / 2) * 1);
	hright = hleft - (float)hwidth;
	htop = (float)(hheight / 2);
	hbottom = htop - (float)hheight;

	std::vector<VertexPosTexNorm> vertices;

	float hSize = 0.5f;

	vertices.push_back({ { hright, htop, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hleft, htop, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hleft, hbottom, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });

	vertices.push_back({ { hleft, hbottom, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hright, hbottom, 0.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } });
	vertices.push_back({ { hright, htop, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } });

	return CreateBuffers(device, vertices.data(), vertices.size());
}
