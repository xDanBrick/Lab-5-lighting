#pragma once
#include "Mesh.h"

class CurvedPlaneMesh : public Mesh
{
public:
	CurvedPlaneMesh(ID3D11Device* device);
	~CurvedPlaneMesh();
};

