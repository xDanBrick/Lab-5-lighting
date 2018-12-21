Texture2D texture1 : register(t0);
SamplerState samplerState;

/////////////
// GLOBALS //
/////////////

cbuffer MaterialBuffer
{
	float3 albedoColor;
	float smoothness;
	float shininess;
	float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
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
	output.color = float4(albedoColor * texture1.Sample(samplerState, input.tex), shininess);
	output.normal = float4(input.normal.xyz, smoothness);
	output.position = float4(input.worldPosition, 1.0f);
	
	return output;
}
