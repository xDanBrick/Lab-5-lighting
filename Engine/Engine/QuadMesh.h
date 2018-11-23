#pragma once
#include "Mesh.h"

class QuadMesh : public Mesh
{
	public:
		QuadMesh(ID3D11Device* device);
		~QuadMesh();
};

