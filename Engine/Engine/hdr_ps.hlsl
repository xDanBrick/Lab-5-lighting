Texture2D sceneTexture : register(t0);
SamplerState samplerState;

/////////////
// GLOBALS //
/////////////

cbuffer TonemapBuffer
{
	float linearExposure;
	int filterType;
	int SRGB;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float3 LinearToSRGB(float3 color)
{
	return pow(abs(color), 1 / 2.2f);
}

float3 ToneMapReinhard(float3 color)
{
	return color / (1.0f + color);
}

float3 SRGBToLinear(float3 srgb)
{
	return pow(abs(srgb), 2.2f);
}

float3 ToneMapACESFilmic(float3 color)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((color*(a*color + b)) / (color*(c*color + d) + e));
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 hdr = sceneTexture.Sample(samplerState, input.tex);
	float3 sdr;
	//None
	if (filterType == 0)
	{
		sdr = hdr.xyz;
	}
	//Saturate
	else if (filterType == 1)
	{
		sdr = saturate(hdr.xyz * linearExposure);
	}
	//Reinhard
	else if (filterType == 2)
	{
		sdr = ToneMapReinhard(hdr.xyz * linearExposure);
	}
	//ACESFilmic
	else if (filterType == 3)
	{
		sdr = ToneMapACESFilmic(hdr.xyz * linearExposure);
	}
	if (SRGB == 1)
	{
		sdr = LinearToSRGB(sdr);
	}
	return float4(sdr, hdr.a);
}
