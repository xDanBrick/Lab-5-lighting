/////////////
// GLOBALS //
/////////////

cbuffer LightBuffer
{
	float4 apexColor;
	float4 centerColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 position2 : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float height = input.position2.y;

	// The value ranges from -1.0f to +1.0f so change it to only positive values.
	if (height < 0.0f)
	{
		height = 0.0f;
	}

	return lerp(centerColor, apexColor, height);
}