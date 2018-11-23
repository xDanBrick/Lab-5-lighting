#pragma once
#include "Mesh.h"

class PlaneMesh : public Mesh
{
	public:
		PlaneMesh(ID3D11Device* device);
		~PlaneMesh();
};

