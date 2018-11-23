#pragma once
#include "Mesh.h"

class OrthoMesh : public Mesh
{
	public:
		OrthoMesh();
		~OrthoMesh();

		bool			Initalise(ID3D11Device* device, int width, int height);
};

