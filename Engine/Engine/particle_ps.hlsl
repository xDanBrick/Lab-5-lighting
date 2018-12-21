Texture2D texture1 : register(t0);
SamplerState samplerState;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	return texture1.Sample(samplerState, input.tex);
}
