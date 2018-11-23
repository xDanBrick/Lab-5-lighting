float4 Normal(float3 pos1, float3 pos2, float3 pos3)
{
	float3 vector1 = float3(pos1.x - pos3.x, pos1.y - pos3.y, pos1.z - pos3.z);
	float3 vector2 = float3(pos3.x - pos2.x, pos3.y - pos2.y, pos3.z - pos2.z);

	//// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
	//normals.push_back({ (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]),
	//	(vector1[2] * vector2[0]) - (vector1[0] * vector2[2]),
	//	(vector1[0] * vector2[1]) - (vector1[1] * vector2[0]) });
	
	float3 normal = float3((vector1.y * vector2.z) - (vector1.z * vector2.y), (vector1.z * vector2.x) - (vector1.x * vector2.z), (vector1.x * vector2.y) - (vector1.y * vector2.x));
	float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	return float4(normal.x / length, normal.y / length, normal.z / length, 1.0f);
}

Texture2D<float4> heightMapTexture : register(t0);
SamplerState sampleType : register(s0);
RWTexture2D<float4> output : register(u0);

[numthreads(16, 16, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	float texScaler = 1.0f / 256.0f;
	float2 index1 = float2((dispatchThreadID.x * texScaler), (dispatchThreadID.y * texScaler));
	float2 index2 = float2((dispatchThreadID.x * texScaler) + texScaler, (dispatchThreadID.y * texScaler));
	float2 index3 = float2((dispatchThreadID.x * texScaler), (dispatchThreadID.y * texScaler) + texScaler);
	
	float3 pos1 = float3((float)dispatchThreadID.x, heightMapTexture.SampleLevel(sampleType, index1, 1).x * 50.0f, (float)dispatchThreadID.y);
	float3 pos2 = float3((float)dispatchThreadID.x + 1.0f, heightMapTexture.SampleLevel(sampleType, index2, 1).x * 50.0f, (float)dispatchThreadID.y);
	float3 pos3 = float3((float)dispatchThreadID.x, heightMapTexture.SampleLevel(sampleType, index3, 1).x * 50.0f, (float)dispatchThreadID.y + 1.0f);

	output[dispatchThreadID.xy] = Normal(pos1, pos2, pos3);
}