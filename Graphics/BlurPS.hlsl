Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float normpdf(float x, float sigma)
{
    return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}


float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float3 c = g_texture0.Sample(g_sampler, float2((input.uv.x + dx), (input.uv.y + dy))).rgb;
		
	//declare stuff
    const int mSize = 5;
    const int kSize = (mSize - 1) / 2;
    float kernel[mSize];
    float3 final_colour = float3(0.f, 0.f, 0.f);
		
	//create the 1-D kernel
    float sigma = 7.f;
    float Z = 0.0;
    for (int k = 0; k <= kSize; ++k)
    {
        kernel[kSize + k] = kernel[kSize - k] = normpdf(float(k), sigma);
    }
		
	//get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j)
    {
        Z += kernel[j];
    }
		
	//read out the texels
    for (int i = -kSize; i <= kSize; ++i)
    {
        for (int j = -kSize; j <= kSize; ++j)
        {
            final_colour += kernel[kSize + j] * kernel[kSize + i] * g_texture0.Sample(g_sampler, (input.uv + float2(dx*i, dy*j))).rgb;
	
        }
        
    }
		
    return float4(final_colour / (Z * Z), 1.0);
}