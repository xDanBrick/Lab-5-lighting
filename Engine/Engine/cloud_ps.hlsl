/////////////
// GLOBALS //
/////////////
Texture2D cloudTexture : register(t0);
Texture2D perturbTexture : register(t1);
SamplerState SampleType;

cbuffer SkyBuffer
{
	float2 translations;
	float2 scales;
	float2 brightness;
	float2 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};



////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 perturbValue;
	float4 cloudColor, cloud2Color;

	float2 transTexCoords = input.tex;
	// Translate the texture coordinate sampling location by the translation value.
	transTexCoords.x = transTexCoords.x + translations.x;

	// Sample the texture value from the perturb texture using the translated texture coordinates.
	perturbValue = perturbTexture.Sample(SampleType, transTexCoords);

	// Multiply the perturb value by the perturb scale.
	perturbValue = perturbValue * scales.x;

	// Add the texture coordinates as well as the translation value to get the perturbed texture coordinate sampling location.
	perturbValue.xy = perturbValue.xy + transTexCoords + translations.x;

	// Now sample the color from the cloud texture using the perturbed sampling coordinates.
	cloudColor = cloudTexture.Sample(SampleType, perturbValue.xy);

	// Reduce the color cloud by the brightness value.
	cloudColor = cloudColor * brightness.x;

	transTexCoords = input.tex;
	// Translate the texture coordinate sampling location by the translation value.
	transTexCoords.x = transTexCoords.x + translations.y;

	// Sample the texture value from the perturb texture using the translated texture coordinates.
	perturbValue = perturbTexture.Sample(SampleType, transTexCoords);

	// Multiply the perturb value by the perturb scale.
	perturbValue = perturbValue * scales.y;

	// Add the texture coordinates as well as the translation value to get the perturbed texture coordinate sampling location.
	perturbValue.xy = perturbValue.xy + transTexCoords + translations.y;

	// Now sample the color from the cloud texture using the perturbed sampling coordinates.
	cloud2Color = cloudTexture.Sample(SampleType, perturbValue.xy);

	// Reduce the color cloud by the brightness value.
	cloud2Color = cloud2Color * brightness.y;

	return lerp(cloudColor, cloud2Color, 0.5f);
}