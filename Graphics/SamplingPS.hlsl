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
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float3 color = g_texture0.Sample(g_sampler, input.texcoord).rgb;
    float l = (color.r + color.g + color.b) / 3;
    
    return l > threshold ? float4(color, 1.0f) : float4(0.0f, 0.0f, 0.0f, 1.0f);
    //return g_texture0.Sample(g_sampler, input.texcoord);
    //return float4(0.0f, 1.0f, 0.0f, 0.0f);
}