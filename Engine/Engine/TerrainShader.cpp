#include "TerrainShader.h"

TerrainShader::TerrainShader()
{
	heightmapTexture_ = nullptr;
	heightmapResourceView_ = nullptr;
	structuredBufferUAV_ = nullptr;
	hullShader_ = nullptr;
	domainShader_ = nullptr;
}


TerrainShader::~TerrainShader()
{
}

bool TerrainShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = 256;
	textureDesc.Height = 256;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, 0, &heightmapTexture_);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
	ZeroMemory(&descUAV, sizeof(descUAV));
	descUAV.Format = DXGI_FORMAT_UNKNOWN;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;
	device->CreateUnorderedAccessView(heightmapTexture_, &descUAV, &structuredBufferUAV_);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(heightmapTexture_, &srvDesc, &heightmapResourceView_);

	terrainBuffer_.Initalise(device);

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = 256;
	textureDesc.Height = 256;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, 0, &normalmapTexture_);

	ZeroMemory(&descUAV, sizeof(descUAV));
	descUAV.Format = DXGI_FORMAT_UNKNOWN;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;
	device->CreateUnorderedAccessView(normalmapTexture_, &descUAV, &normalmapTextureUAV_);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(normalmapTexture_, &srvDesc, &normalmapResourceView_);

	////////////////////////////////////////////////////////////////////////////////////////

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = 256;
	textureDesc.Height = 256;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, 0, &smoothNormalmapTexture_);

	ZeroMemory(&descUAV, sizeof(descUAV));
	descUAV.Format = DXGI_FORMAT_UNKNOWN;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;
	device->CreateUnorderedAccessView(smoothNormalmapTexture_, &descUAV, &smoothNormalmapUAV_);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(smoothNormalmapTexture_, &srvDesc, &smoothNormalmapResourceView_);

	if (!computeShader_.Initalise(device, hwnd, L"../Engine/TerrainCompute.hlsl"))
	{
		return false;
	}

	if (!normalComputeShader_.Initalise(device, hwnd, L"../Engine/NormalCompute.hlsl"))
	{
		return false;
	}

	if (!smoothNormalCompute_.Initalise(device, hwnd, L"../Engine/SmoothNormalCompute.hlsl"))
	{
		return false;
	}

	if (!terrainPSBuffer_.Initalise(device))
	{
		return false;
	}

	auto shaderBuffer = D3DGraphics::CompileShader(device, hwnd, L"../Engine/terrain_hs.hlsl", "hs_5_0");
	if (!shaderBuffer)
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &hullShader_)))
	{
		return false;
	}

	shaderBuffer = D3DGraphics::CompileShader(device, hwnd, L"../Engine/terrain_ds.hlsl", "ds_5_0");
	if (!shaderBuffer)
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &domainShader_)))
	{
		return false;
	}

	return InitializeShader(device, hwnd, L"../Engine/terrain_vs.hlsl", L"../Engine/terrain_ps.hlsl");
}

void TerrainShader::Shutdown()
{
	ReleaseNull(structuredBufferUAV_);
	ReleaseNull(heightmapResourceView_);
	ReleaseNull(heightmapTexture_);
	ReleaseNull(hullShader_);
	ReleaseNull(domainShader_);
	Shader::Shutdown();
}

bool TerrainShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* texture, float perlinScale, float persistance, float octaves, float slopeHeight, bool& updateHeightmap)
{
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
		deviceContext->CSSetUnorderedAccessViews(0, 1, &structuredBufferUAV_, &initCounts);

		//Set the Compute shader
		computeShader_.Dispatch(deviceContext, 16, 16);
		
		deviceContext->CSSetUnorderedAccessViews(0, 1, &normalmapTextureUAV_, &initCounts);
		deviceContext->CSSetShaderResources(0, 1, &heightmapResourceView_);

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
	Shader::SetBaseParameters(deviceContext, mesh, world, view, projection);
	return true;
}
