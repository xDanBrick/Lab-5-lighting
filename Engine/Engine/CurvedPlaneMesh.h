#pragma once
#include "Mesh.h"

class CurvedPlaneMesh : public Mesh
{
public:
	CurvedPlaneMesh();
	~CurvedPlaneMesh();
	bool Initalise(ID3D11Device* device);
};

