struct ParticleData
{
	float3 position;
};

StructuredBuffer<ParticleData>			inputConstantParticleData   : register(t0);
RWStructuredBuffer<ParticleData>        outputParticleData          : register(u0);

[numthreads(100, 1, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	float3 position = inputConstantParticleData[dispatchThreadID.x].position;
	if (position.y < 0.0f)
	{
		position.y = 50.0f;
	}
	else
	{
		position.y -= 1.0f;
	}
	outputParticleData[dispatchThreadID.x].position = position;
}