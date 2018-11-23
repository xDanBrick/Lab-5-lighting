#include "CurvedPlaneMesh.h"
#include <vector>

void CreateCurvedPlane(std::vector<VertexPosTexNorm>& vertices, float size, int tesselation, int textureRepeat)
{
	float skyPlaneTop, skyPlaneBottom;
	skyPlaneTop = 50.0f;
	skyPlaneBottom = -10.0f;
	
	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};

	float quadSize, radius, constant, textureDelta;
	int i, j, index;
	float positionX, positionY, positionZ, tu, tv;

	// Create the array to hold the sky plane coordinates.
	auto skyPlane = new SkyPlaneType[(tesselation + 1) * (tesselation + 1)];

	// Determine the size of each quad on the sky plane.
	quadSize = size / (float)tesselation;

	// Calculate the radius of the sky plane based on the width.
	radius = size / 2.0f;

	// Calculate the height constant to increment by.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// Calculate the texture coordinate increment value.
	textureDelta = (float)textureRepeat / (float)tesselation;

	// Loop through the sky plane and build the coordinates based on the increment values given.
	for (j = 0; j <= tesselation; j++)
	{
		for (i = 0; i <= tesselation; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * size) + ((float)i * quadSize);
			positionZ = (-0.5f * size) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// Calculate the texture coordinates.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// Calculate the index into the sky plane array to add this coordinate.
			index = j * (tesselation + 1) + i;

			// Add the coordinates to the sky plane array.
			skyPlane[index].x = positionX;
			skyPlane[index].y = positionY - 0.1f;
			skyPlane[index].z = positionZ;
			skyPlane[index].tu = tu;
			skyPlane[index].tv = tv;
		}
	}
	int index1, index2, index3, index4;
	// Initialize the index into the vertex array.
	index = 0;

	// Load the vertex and index array with the sky plane array data.
	for (j = 0; j<tesselation; j++)
	{
		for (i = 0; i<tesselation; i++)
		{
			index1 = j * (tesselation + 1) + i;
			index2 = j * (tesselation + 1) + (i + 1);
			index3 = (j + 1) * (tesselation + 1) + i;
			index4 = (j + 1) * (tesselation + 1) + (i + 1);

			/*indices.push_back(index1);
			indices.push_back(index2);
			indices.push_back(index3);
			indices.push_back(index3);
			indices.push_back(index2);
			indices.push_back(index4);*/
			// Triangle 1 - Upper Left
			vertices.push_back({ { skyPlane[index1].x, skyPlane[index1].y, skyPlane[index1].z } ,{ skyPlane[index1].tu, skyPlane[index1].tv },{ 0.0f, -1.0f, 0.0f } });
			vertices.push_back({ { skyPlane[index2].x, skyPlane[index2].y, skyPlane[index2].z },{ skyPlane[index2].tu, skyPlane[index2].tv },{ 0.0f, -1.0f, 0.0f } });
			vertices.push_back({ { skyPlane[index3].x, skyPlane[index3].y, skyPlane[index3].z },{ skyPlane[index3].tu, skyPlane[index3].tv },{ 0.0f, -1.0f, 0.0f } });
			vertices.push_back({ { skyPlane[index3].x, skyPlane[index3].y, skyPlane[index3].z },{ skyPlane[index3].tu, skyPlane[index3].tv },{ 0.0f, -1.0f, 0.0f } });
			vertices.push_back({ { skyPlane[index2].x, skyPlane[index2].y, skyPlane[index2].z },{ skyPlane[index2].tu, skyPlane[index2].tv },{ 0.0f, -1.0f, 0.0f } });
			vertices.push_back({ { skyPlane[index4].x, skyPlane[index4].y, skyPlane[index4].z },{ skyPlane[index4].tu, skyPlane[index4].tv },{ 0.0f, -1.0f, 0.0f } });
		}
	}
}

CurvedPlaneMesh::CurvedPlaneMesh(ID3D11Device* device)
{
	//64, 20.0f, 1.0f, 1.0f
	std::vector<VertexPosTexNorm> vertices;
	CreateCurvedPlane(vertices, 512.0f, 128, 1);
	CreateBuffers(device, vertices.data(), vertices.size());
}


CurvedPlaneMesh::~CurvedPlaneMesh()
{
}
