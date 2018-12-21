#include "TerrainShader.h"

const int TERRAIN_DIMENSIONS = 256;

TerrainShader::TerrainShader()
{
	heightmapTexture_ = nullptr;
	heightmapResourceView_ = nullptr;
}


TerrainShader::~TerrainShader()
{
}

bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	//Create heightmap texture and resources
	heightmapTexture_ = D3DGraphics::CreateTexture2D(device, TERRAIN_DIMENSIONS, TERRAIN_DIMENSIONS);
	heightmapTextureUAV_ = D3DGraphics::CreateTextureUnorderedAccessView(device, heightmapTexture_);
	heightmapResourceView_ = D3DGraphics::CreateShaderResourceView(device, heightmapTexture_);

	//Create normalmap texture and resources
	normalmapTexture_ = D3DGraphics::CreateTexture2D(device, TERRAIN_DIMENSIONS, TERRAIN_DIMENSIONS);
	normalmapTextureUAV_ = D3DGraphics::CreateTextureUnorderedAccessView(device, normalmapTexture_);
	normalmapResourceView_ = D3DGraphics::CreateShaderResourceView(device, normalmapTexture_);

	terrainBuffer_.Initalise(device);

	//Create compute shader for calc heightmap
	if (!computeShader_.Initalise(device, hwnd, L"../Engine/TerrainCompute.hlsl"))
	{
		return false;
	}

	//Create compute shader for calc normalmap
	if (!normalComputeShader_.Initalise(device, hwnd, L"../Engine/NormalCompute.hlsl"))
	{
		return false;
	}

	if (!terrainPSBuffer_.Initalise(device))
	{
		return false;
	}
	return InitializeShader(device, hwnd, L"../Engine/terrain_vs.hlsl", L"../Engine/terrain_ps.hlsl");
}

void TerrainShader::Shutdown()
{
	ReleaseNull(heightmapTextureUAV_);
	ReleaseNull(heightmapResourceView_);
	ReleaseNull(heightmapTexture_);
	Shader::Shutdown();
}

bool TerrainShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* texture, float perlinScale, float persistance, float octaves, float slopeHeight, bool& updateHeightmap)
{
	//If the heightmap needs updated
	if (updateHeightmap)
	{
		terrainBuffer_.getValuePtr()->octaves = octaves;
		terrainBuffer_.getValuePtr()->perlinScale = perlinScale;
		terrainBuffer_.getValuePtr()->persistence = persistance;
		terrainBuffer_.getValuePtr()->padding = 0.0f;
		terrainBuffer_.Map(deviceContext);
		
		UINT initCounts = 0;
		auto terrainBuffer = terrainBuffer_.getBuffer();
		deviceContext->CSSetConstantBuffers(0, 1, &terrainBuffer);
		deviceContext->CSSetUnorderedAccessViews(0, 1, &heightmapTextureUAV_, &initCounts);

		//Run the compute shader for calc heightmap
		computeShader_.Dispatch(deviceContext, 16, 16);
		
		deviceContext->CSSetUnorderedAccessViews(0, 1, &normalmapTextureUAV_, &initCounts);
		deviceContext->CSSetShaderResources(0, 1, &heightmapResourceView_);

		//Run the compute shader for calc normalmap
		normalComputeShader_.Dispatch(deviceContext, 16, 16);
		updateHeightmap = false;
	}

	deviceContext->VSSetShaderResources(0, 1, &heightmapResourceView_);
	deviceContext->VSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &normalmapResourceView_);
	terrainPSBuffer_.getValuePtr()->slopeStart = slopeHeight;
	terrainPSBuffer_.Map(deviceContext);

	auto terrainBuffer = terrainPSBuffer_.getBuffer();
	deviceContext->PSSetConstantBuffers(0, 1, &terrainBuffer);

	return Shader::SetBaseParameters(deviceContext, mesh, world, view, projection);
}
