#include "PostShader.h"



PostShader::PostShader()
{
}


PostShader::~PostShader()
{
}

bool PostShader::Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight)
{
	for (int i = 0; i < ARRAYSIZE(defferedTextures_); ++i)
	{
		if (!defferedTextures_[i].Initialize(device, screenWidth, screenHeight))
		{
			return false;
		}
	}

	if (!defferedComputeShader_.Initalise(device, hwnd, L"../Engine/DefferedCompute.hlsl"))
	{
		return false;
	}

	if (!defferedBuffer_.Initalise(device))
	{
		return false;
	}

	if (!lightBuffer_.Initalise(device))
	{
		return false;
	}

	if (!tonemapBuffer_.Initalise(device))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = screenWidth;
	textureDesc.Height = screenHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, 0, &defferedTexture_);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
	ZeroMemory(&descUAV, sizeof(descUAV));
	descUAV.Format = DXGI_FORMAT_UNKNOWN;
	descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descUAV.Texture2D.MipSlice = 0;
	device->CreateUnorderedAccessView(defferedTexture_, &descUAV, &defferedTextureUAV_);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(defferedTexture_, &srvDesc, &defferedResourceView_);

	for (int i = 0; i < LIGHT_COUNT; ++i)
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

	return InitializeShader(device, hwnd, L"../Engine/post_vs.hlsl", L"../Engine/post_ps.hlsl");
}

void PostShader::Shutdown()
{
	Shader::Shutdown();
}

static float inc = 0.0f;

bool PostShader::Render(ID3D11DeviceContext * deviceContext, Mesh* mesh, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, D3DXVECTOR3 cameraPositon, int lightCount, TonemapType mapType, float exposure, bool sRGB, D3DXVECTOR3 lightDirection)
{
	if (lightCount > LIGHT_COUNT)
	{
		lightCount = LIGHT_COUNT;
	}
	UINT initCounts = 0;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &defferedTextureUAV_, &initCounts);
	for (int i = 0; i < 3; ++i)
	{
		auto rtV = defferedTextures_[i].GetShaderResourceView();
		deviceContext->CSSetShaderResources(i, 1, &rtV);
	}

	tonemapBuffer_.getValuePtr()->filterType = (int)mapType;
	tonemapBuffer_.getValuePtr()->linearExposure = (int)exposure;
	tonemapBuffer_.getValuePtr()->SRGB = (int)sRGB;
	tonemapBuffer_.Map(deviceContext);

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
	lightBuffer_.Map(deviceContext);

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

void PostShader::setDefferedRendering(ID3D11DeviceContext * deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	ID3D11RenderTargetView* renderTargets[3];

	for (int i = 0; i < 3; ++i)
	{
		defferedTextures_[i].ClearRenderTarget(deviceContext, depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);
		renderTargets[i] = defferedTextures_[i].getRenderTarget();
	}
	
	deviceContext->OMSetRenderTargets(3, renderTargets, depthStencilView);
}
