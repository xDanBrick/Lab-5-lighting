#define Pi 3.14159

float2 Hammersley(uint i, uint N)
{
	float ri = reversebits(i) * 2.3283064365386963e-10f;
	return float2(float(i) / float(N), ri);
}

// ===============================================================================================
// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
// ===============================================================================================
float GGX(float NdotV, float a)
{
	float k = a / 2;
	return NdotV / (NdotV * (1.0f - k) + k);
}

// ===============================================================================================
// Geometry Term
// -----------------------------------------------------------------------------------------------
// Defines the shadowing from the microfacets.
//
// Smith approximation:
// http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
// http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
//
// ===============================================================================================
float G_Smith(float a, float nDotV, float nDotL)
{
	return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}

// ===============================================================================================
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf 
// ===============================================================================================
float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
	float a = Roughness * Roughness; // DISNEY'S ROUGHNESS [see Burley'12 siggraph]

	float Phi = 2 * Pi * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 Schlick_Fresnel(float3 f0, float3 h, float3 l)
{
	return f0 + (1.0f - f0) * pow((1.0f - dot(l, h)), 5.0f);
}

struct LightBufferType
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 lightPosition;
	float4 lightDirection;
	float4 spotAngle;
	float4 lightType;
	float4 attenuation;
	float4 padding;
};

float3 DirectDiffuseBRDF(float3 diffuseAlbedo, float nDotL)
{
	return (diffuseAlbedo * nDotL) / Pi;
}

// ================================================================================================
// Cook-Torrence Microfacet BRDF
// ------------------------------------------------------------------------------------------------
// f = D * F * G / (4 * (N.L) * (N.V))
// 
// D = Normal Distribution
// F = Fresnel
// G = Geometry Term
//
// ================================================================================================
float3 DirectSpecularBRDF(float roughness, float3 specularAlbedo, float3 positionWS, float3 normalWS, float3 lightDir, float3 cameraPosition)
{
	float3 viewDir = normalize(cameraPosition - positionWS);
	float3 halfVec = normalize(viewDir + lightDir);

	float nDotH = saturate(dot(normalWS, halfVec));
	float nDotL = saturate(dot(normalWS, lightDir));
	float nDotV = max(dot(normalWS, viewDir), 0.0001f);

	float alpha2 = roughness * roughness;

	// Computes the distribution of the microfacets for the shaded surface.
	// Trowbridge-Reitz/GGX normal distribution function.
	float  D = alpha2 / (Pi * pow(nDotH * nDotH * (alpha2 - 1) + 1, 2.0f));

	// Computes the amount of light that reflects from a mirror surface given its index of refraction. 
	// Schlick's approximation.
	float3 F = Schlick_Fresnel(specularAlbedo, halfVec, lightDir);

	// Computes the shadowing from the microfacets.
	// Smith's approximation.
	float  G = G_Smith(roughness, nDotV, nDotL);

	return D * F * G;
}

float4 CalulateLight(LightBufferType light, float3 texureColor, float3 normals, float3 position, float3 specularAlbedo, float roughness, float3 cameraPosition)
{
	float att = 1.0f;
	float spot = 1.0f;
	float lightIntensity;
	float3 lightDir, lighting;

	if (light.lightType.x == -1.0f)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	//Directional
	else if (light.lightType.x == 1.0f)
	{
		lightDir = normalize(light.lightDirection).xyz;
		lightDir = -lightDir;
		lightIntensity = saturate(dot(normals, lightDir));
	}
	//Point
	else
	{
		float3 pixelToLight = light.lightPosition.xyz - position;
		float lightDist = length(pixelToLight);
		lightDir = pixelToLight / lightDist;
		att = 1.0f / (0.0f + light.attenuation.x * lightDist + 0.0f * (lightDist * lightDist));
		lightIntensity = saturate(dot(normals, lightDir));

		//Spot
		if (light.lightType.x == 2.0f)
		{
			float minCos = cos(light.spotAngle.x);
			float maxCos = (minCos + 1.0f) / 2.0f;
			float cosAngle = dot(lightDir, -lightIntensity);
			spot = smoothstep(minCos, maxCos, cosAngle);
		}
	}
	if (lightIntensity > 0.0f)
	{
		float3 lighting = DirectDiffuseBRDF(texureColor, lightIntensity) + DirectSpecularBRDF(roughness, specularAlbedo, position, normals, lightDir, cameraPosition);
		return float4(max(lighting, 0.0f), 1.0f) * light.diffuseColor * spot * att;
	}
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D positionTexture : register(t2);

#define LIGHT_COUNT 512

cbuffer LightBuffer : register(b0)
{
	LightBufferType lights[LIGHT_COUNT];
};

cbuffer DefferedBuffer : register(b1)
{
	float3 cameraPosition;
	int lightCount;
};

SamplerState sampleType : register(s0);
RWTexture2D<float4> output : register(u0);

[numthreads(10, 10, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	float2 texCoors = float2(dispatchThreadID.x / 800.0f, dispatchThreadID.y / 600.0f);
	float4 normals = normalTexture.SampleLevel(sampleType, texCoors, 1.0f);
	float4 diffuseColor = diffuseTexture.SampleLevel(sampleType, texCoors, 1.0f);

	if (normals.x == 0.0f &&  normals.y == 0.0f && normals.z == 0.0f)
	{
		output[dispatchThreadID.xy] = diffuseColor;
	}
	else
	{
		float4 pos = positionTexture.SampleLevel(sampleType, texCoors, 1.0f);
		float4 color = lights[0].ambientColor;
		for (unsigned int i = 0; i < lightCount; ++i)
		{
			color += CalulateLight(lights[i], diffuseColor.xyz, normals.xyz, pos.xyz, float3(diffuseColor.w, diffuseColor.w, diffuseColor.w), normals.w, cameraPosition);
		}
		
		output[dispatchThreadID.xy] = color;
	}
}