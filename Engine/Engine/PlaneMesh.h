#pragma once
#include "Mesh.h"

class PlaneMesh : public Mesh
{
	public:
		PlaneMesh();
		~PlaneMesh();
		bool Initalise(ID3D11Device* device);
};

