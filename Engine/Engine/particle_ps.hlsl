////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

//cbuffer LightBuffer
//{
//	float4 ambientLight;
//    float4 diffuseColor;
//    float3 lightDirection;
//	float padding;
//};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{

	//float3 lightDir;
	//float lightIntensity;
	//float4 color = ambientLight;

	//// Invert the light direction for calculations.
 //   lightDir = -lightDirection;

 //   // Calculate the amount of light on this pixel.
 //   lightIntensity = saturate(dot(input.normal, lightDir));

	//if (lightIntensity > 0.0f)
	//{
	//	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	//	color += saturate(diffuseColor * lightIntensity);
	//}

    return float4(1.0f, 1.0f, 1.0f, 0.5f);
}
