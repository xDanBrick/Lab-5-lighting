Texture2D texture1 : register(t0);
Texture2D normalMap : register(t1);
SamplerState samplerState;
/////////////
// GLOBALS //
/////////////

cbuffer TerrainBuffer
{
	float slopeStart;
	float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 worldPosition : POSITION;
};

struct PixelOutputDeffered
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PixelOutputDeffered main(PixelInputType input) : SV_TARGET
{
	PixelOutputDeffered output;
	float4 normals = normalMap.Sample(samplerState, input.tex);
	if ((1.0f - normals.y) > slopeStart)
	{
		output.color = texture1.Sample(samplerState, input.tex * float2(10.0f, 10.0f));
		output.color.w = 0.0f;
	}
	else
	{
		output.color = float4(1.0f, 1.0f, 1.0f, 0.0f);
	}
	output.position = float4(input.worldPosition, 1.0f);
	output.normal = float4(normals.xyz, 0.0f);
	return output;
}
