// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D heightMapTexture : register(t0);
SamplerState sampleType : register(s0);

cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 position3D : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 texturePosition;
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, 1 - uvwCoord.y);
	float3 v2 = lerp(patch[2].position, patch[3].position, 1 - uvwCoord.y);

	float2 t1 = lerp(patch[0].tex, patch[1].tex, 1 - uvwCoord.y);
	float2 t2 = lerp(patch[2].tex, patch[3].tex, 1 - uvwCoord.y);

	vertexPosition = lerp(v1, v2, uvwCoord.x);
	texturePosition = lerp(t1, t2, uvwCoord.x);

	vertexPosition.y = (heightMapTexture.SampleLevel(sampleType, texturePosition, 1));
	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = texturePosition;
	output.position3D = mul(vertexPosition, worldMatrix);
	return output;
}