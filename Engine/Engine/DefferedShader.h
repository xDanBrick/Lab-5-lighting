#pragma once
#include "Shader.h"
#include "ComputeShader.h"
#include "rendertextureclass.h"

const unsigned int LIGHT_MAX = 512;
const unsigned int TONEMAP_MAX = 3;
const int DEFFERED_RT_COUNT = 3;

class DefferedShader : public Shader
{
public:
	DefferedShader();
	~DefferedShader();

	bool			Initialize(ID3D11Device*, HWND, int screenWidth, int screenHeight);
	void			Shutdown() override;
	bool			Render(ID3D11DeviceContext*, Mesh* mesh, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR3 cameraPositon, int lightCount, int mapType, float exposure, bool sRGB, D3DXVECTOR3 lightDirection);
	void			setDefferedRendering(ID3D11DeviceContext* device, ID3D11DepthStencilView* depthStencilView);
private:
	ComputeShader	defferedComputeShader_;
	RenderTextureClass	 defferedTextures_[DEFFERED_RT_COUNT];

	ID3D11UnorderedAccessView*				defferedTextureUAV_;
	ID3D11Texture2D*						defferedTexture_;
	ID3D11ShaderResourceView*				defferedResourceView_;

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 lightPosition;
		D3DXVECTOR4 lightDirection;
		D3DXVECTOR4 spotAngle;
		D3DXVECTOR4 lightType;
		D3DXVECTOR4 attenuation;
		D3DXVECTOR4 padding;
	};

	struct LightBuffer
	{
		LightBufferType lights[LIGHT_MAX];
	};

	struct DefferedBuffer
	{
		D3DXVECTOR3 camPos;
		int lightCount;
	};

	struct TonemapBuffer
	{
		float linearExposure;
		int filterType;
		int SRGB;
		float padding;
	};

	ConstantBuffer<DefferedBuffer>			defferedBuffer_;
	ConstantBuffer<LightBuffer>				lightBuffer_;
	ConstantBuffer<TonemapBuffer>			tonemapBuffer_;
};