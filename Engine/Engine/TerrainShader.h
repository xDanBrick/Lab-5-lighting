#pragma once
#include "Shader.h"
#include "ComputeShader.h"

class TerrainShader : public Shader
{
public:
	TerrainShader();
	~TerrainShader();

	bool			Initialize(ID3D11Device*, HWND hwnd);
	void			Shutdown() override;
	bool			Render(ID3D11DeviceContext*, Mesh* mesh, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView* texture, float perlinScale, float persistance, float octaves, float slopeHeight, bool& updateHeightmap);

private:
	struct TerrainBuffer
	{
		float perlinScale;
		float persistence;
		float octaves;
		float padding;
	};

	struct TerrainPSBuffer
	{
		float slopeStart;
		D3DXVECTOR3 padding;
	};

	ComputeShader							computeShader_, normalComputeShader_;
	ID3D11UnorderedAccessView*				heightmapTextureUAV_, *normalmapTextureUAV_;
	ID3D11Texture2D*						heightmapTexture_, *normalmapTexture_;
	ID3D11ShaderResourceView*				heightmapResourceView_, *normalmapResourceView_;

	ConstantBuffer<TerrainBuffer>			terrainBuffer_;
	ConstantBuffer<TerrainPSBuffer>			terrainPSBuffer_;
};

