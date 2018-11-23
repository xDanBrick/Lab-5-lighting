#pragma once
#include "Shader.h"
#include "ComputeShader.h"
#include "rendertextureclass.h"

const unsigned int LIGHT_COUNT = 512;

enum class TonemapType { none, saturate, reinhard, filmic };

class PostShader : public Shader
{
public:
	PostShader();
	~PostShader();

	bool			Initialize(ID3D11Device*, HWND, int screenWidth, int screenHeight);
	void			Shutdown() override;
	bool			Render(ID3D11DeviceContext*, Mesh* mesh, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR3 cameraPositon, int lightCount, TonemapType mapType, float exposure, bool sRGB, D3DXVECTOR3 lightDirection);
	void			setDefferedRendering(ID3D11DeviceContext* device, ID3D11DepthStencilView* depthStencilView);
private:
	ComputeShader	defferedComputeShader_;
	RenderTextureClass	 defferedTextures_[3];

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
		LightBufferType lights[512];
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