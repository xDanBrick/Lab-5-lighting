#define PI 3.14159

float Noise2D(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.0 - ((n * ((n * n * 15731) + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float CosineInterpolateNoise2D(float a, float b, float x)
{
	float ft = x * PI;
	float f = (1 - cos(ft)) * 0.5;
	float result = a * (1 - f) + b * f;
	return result;
}

float SmoothNoise2D(float x, float y)
{
	float corners = (Noise2D(x - 1, y - 1) + Noise2D(x + 1, y - 1) + Noise2D(x - 1, y + 1) + Noise2D(x + 1, y + 1)) / 16;
	float sides = (Noise2D(x - 1, y) + Noise2D(x + 1, y) + Noise2D(x, y - 1) + Noise2D(x, y + 1)) / 8;
	float center = Noise2D(x, y) / 4;

	return corners + sides + center;
}

float InterpolatedNoise2D(float x, float y)
{
	int x_whole = (int)x;
	float x_frac = x - x_whole;

	int y_whole = (int)y;
	float y_frac = y - y_whole;

	float v1 = SmoothNoise2D(x_whole, y_whole);
	float v2 = SmoothNoise2D(x_whole, y_whole + 1);
	float v3 = SmoothNoise2D(x_whole + 1, y_whole);
	float v4 = SmoothNoise2D(x_whole + 1, y_whole + 1);

	float i1 = CosineInterpolateNoise2D(v1, v3, x_frac);
	float i2 = CosineInterpolateNoise2D(v2, v4, x_frac);

	return CosineInterpolateNoise2D(i1, i2, y_frac);
}


float CalcPerlinNoise(float x, float y, int i)
{
	float frequency = pow(2, i);
	float amplitude = pow(0.1f, i);
	return InterpolatedNoise2D(x * frequency, y * frequency);
}

float PerlinNoise2D(float x, float y, int octaves)
{
	float total = 0.0f;
	if (octaves > 2)
	{
		for (int i = 0; i < octaves - 1; i++)
		{
			total += CalcPerlinNoise(x, y, i);
		}
	}
	else
	{
		return CalcPerlinNoise(x, y, 0);
	}
	
	return total;
}



RWTexture2D<float4> output : register(u0);

cbuffer NoiseBuffer
{
	float perlinScale;
	float persistence;
	float octaves;
	float padding;
};

[numthreads(16, 16, 1)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	float value = pow((PerlinNoise2D((float)dispatchThreadID.x / perlinScale, (float)dispatchThreadID.y / perlinScale, octaves) * 0.5f + 0.5f), persistence);
	output[dispatchThreadID.xy] = float4(value, value, value, 1.0f);
	//DiamondSquare(dispatchThreadID.xy, 1.8f, 16384 * 4);
}