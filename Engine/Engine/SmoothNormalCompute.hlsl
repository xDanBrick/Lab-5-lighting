RWTexture2D<float4> output : register(u0);
Texture2D<float4> normalmap : register(t0);
SamplerState sampleType : register(s0);

#define SIZE 256

float4 Normal(int2 dispatchThreadID)
{
	int2 index;
	// Initialize the sum.
	float3 sum = float3(0.0f, 0.0f, 0.0f);

	// Initialize the count.
	int count = 0;

	// Bottom left face.
	if (((dispatchThreadID.x - 1) >= 0) && ((dispatchThreadID.y - 1) >= 0))
	{
		index = int2((dispatchThreadID.x - 1), ((dispatchThreadID.y - 1) * (SIZE - 1)));

		sum += normalmap.SampleLevel(sampleType, index, 1).xyz;
		count++;
	}

	// Bottom right face.
	if ((dispatchThreadID.x < (SIZE - 1)) && ((dispatchThreadID.y - 1) >= 0))
	{
		index = int2(dispatchThreadID.x, ((dispatchThreadID.y - 1) * (SIZE - 1)));

		sum += normalmap.SampleLevel(sampleType, index, 1).xyz;
		count++;
	}

	// Upper left face.
	if (((dispatchThreadID.x - 1) >= 0) && (dispatchThreadID.y < (SIZE - 1)))
	{
		index = int2((dispatchThreadID.x - 1), (dispatchThreadID.y * (SIZE - 1)));

		sum += normalmap.SampleLevel(sampleType, index, 1).xyz;
		count++;
	}

	// Upper right face.
	if ((dispatchThreadID.x < (SIZE - 1)) && (dispatchThreadID.y < (SIZE - 1)))
	{
		index = int2(dispatchThreadID.x, (dispatchThreadID.y * (SIZE - 1)));

		sum += normalmap.SampleLevel(sampleType, index, 1).xyz;
		count++;
	}

	// Take the average of the faces touching this vertex.
	sum = float3( (sum.x / (float)count), (sum.y / (float)count), (sum.z / (float)count));

	// Calculate the length of this normal.
	float length = sqrt((sum.x * sum.x) + (sum.y * sum.y) + (sum.z * sum.z));

	// Get an index to the vertex location in the height map array.
	index = int2(dispatchThreadID.x, (dispatchThreadID.y * SIZE));

	// Normalize the final shared normal for this vertex and store it in the height map array.
	return float4((sum.x / length), (sum.y / length), (sum.z / length), 1.0f);
}

[numthreads(16, 16, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	output[dispatchThreadID.xy] = Normal(dispatchThreadID.xy);
}