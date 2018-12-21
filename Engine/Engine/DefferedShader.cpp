#include "DefferedShader.h"

DefferedShader::DefferedShader()
{
}


DefferedShader::~DefferedShader()
{
}

bool DefferedShader::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight)
{
	//Create the render textures for deffered rendering
	for (int i = 0; i < ARRAYSIZE(defferedTextures_); ++i)
	{
		if (!defferedTextures_[i].Initialize(device, screenWidth, screenHeight))
		{
			return false;
		}
	}

	//Initalise the compute shader for deffered lighting
	if (!defferedComputeShader_.Initalise(device, hwnd, L"../Engine/DefferedCompute.hlsl"))
	{
		return false;
	}

	//Initalise the deffered buffer
	if (!defferedBuffer_.Initalise(device))
	{
		return false;
	}

	if (!lightBuffer_.Initalise(device))
	{
		return false;
	}

	//Init the tonemap buffer
	if (!tonemapBuffer_.Initalise(device))
	{
		return false;
	}

	//Create the texture for the deffered pass
	defferedTexture_ = D3DGraphics::CreateTexture2D(device, screenWidth, screenHeight);
	defferedTextureUAV_ = D3DGraphics::CreateTextureUnorderedAccessView(device, defferedTexture_);
	defferedResourceView_ = D3DGraphics::CreateShaderResourceView(device, defferedTexture_);

	//For all the lights that could be in the scene
	for (int i = 0; i < LIGHT_MAX; ++i)
	{
		lightBuffer_.getValuePtr()->lights[i].ambientColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		lightBuffer_.getValuePtr()->lights[i].lightDirection = { -0.5f, -0.5f, 0.0f, 1.0f };
		if (i > 0)
		{
			lightBuffer_.getValuePtr()->lights[i].lightType = { 0.0f, 0.0f, 0.0f, 1.0f };
			lightBuffer_.getValuePtr()->lights[i].diffuseColor = { (float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f, 1.0f };
		}
		else
		{
			lightBuffer_.getValuePtr()->lights[i].lightType = { 1.0f, 1.0f, 1.0f, 1.0f };
			lightBuffer_.getValuePtr()->lights[i].diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
		lightBuffer_.getValuePtr()->lights[i].lightPosition = { (float)(rand() % 512) - 256.0f, 1.5f, (float)(rand() % 512) - 256.0f, 0.0f };
		lightBuffer_.getValuePtr()->lights[i].attenuation = { 0.1f, 0.1f, 0.1f, 1.0f };
	}

	return InitializeShader(device, hwnd, L"../Engine/hdr_vs.hlsl", L"../Engine/hdr_ps.hlsl");
}

void DefferedShader::Shutdown()
{
	Shader::Shutdown();
}

static float inc = 0.0f;

bool DefferedShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, D3DXVECTOR3 cameraPositon, int lightCount, int mapType, float exposure, bool sRGB, D3DXVECTOR3 lightDirection)
{
	if (mapType > TONEMAP_MAX)
	{
		mapType = TONEMAP_MAX;
	}
	if (lightCount > LIGHT_MAX)
	{
		lightCount = LIGHT_MAX;
	}
	UINT initCounts = 0;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &defferedTextureUAV_, &initCounts);
	for (int i = 0; i < ARRAYSIZE(defferedTextures_); ++i)
	{
		auto rtV = defferedTextures_[i].GetShaderResourceView();
		deviceContext->CSSetShaderResources(i, 1, &rtV);
	}

	tonemapBuffer_.getValuePtr()->filterType = (int)mapType;
	tonemapBuffer_.getValuePtr()->linearExposure = (int)exposure;
	tonemapBuffer_.getValuePtr()->SRGB = (int)sRGB;
	tonemapBuffer_.Map(deviceContext);

	//Update all the lights that are currently being used
	for (int i = 0; i < lightCount; ++i)
	{
		if (i > 0)
		{
			if (lightBuffer_.getValuePtr()->lights[i].lightPosition.x > 256.0f)
			{
				lightBuffer_.getValuePtr()->lights[i].lightPosition.x = -256.0f;
			}
			else
			{
				lightBuffer_.getValuePtr()->lights[i].lightPosition.x += 0.1f;
			}
		}
		else
		{
			lightBuffer_.getValuePtr()->lights[i].lightDirection = { lightDirection.x, lightDirection.y, lightDirection.z, 0.0f };
		}
	}
	//Map the light buffer
	lightBuffer_.Map(deviceContext);

	//Set the light buffer to the compute shader
	auto buffer = lightBuffer_.getBuffer();
	deviceContext->CSSetConstantBuffers(0, 1, &buffer);

	defferedBuffer_.getValuePtr()->camPos = cameraPositon;
	defferedBuffer_.getValuePtr()->lightCount = lightCount;
	defferedBuffer_.Map(deviceContext);

	buffer = defferedBuffer_.getBuffer();
	deviceContext->CSSetConstantBuffers(1, 1, &buffer);

	defferedComputeShader_.Dispatch(deviceContext, 80, 60);

	deviceContext->PSSetShaderResources(0, 1, &defferedResourceView_);

	buffer = tonemapBuffer_.getBuffer();
	deviceContext->PSSetConstantBuffers(0, 1, &buffer);

	Shader::SetBaseParameters(deviceContext, mesh, world, view, projection);
	return true;
}

void DefferedShader::setDefferedRendering(ID3D11DeviceContext * deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	ID3D11RenderTargetView* renderTargets[3];

	for (int i = 0; i < 3; ++i)
	{
		defferedTextures_[i].ClearRenderTarget(deviceContext, depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);
		renderTargets[i] = defferedTextures_[i].getRenderTarget();
	}
	
	deviceContext->OMSetRenderTargets(3, renderTargets, depthStencilView);
}
