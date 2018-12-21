Texture2D texture1 : register(t0);
SamplerState samplerState;

/////////////
// GLOBALS //
/////////////

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
	output.color = texture1.Sample(samplerState, input.tex) * float4(0.5f, 0.5f, 0.5f, 0.5f);
	output.normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	return output;
}
