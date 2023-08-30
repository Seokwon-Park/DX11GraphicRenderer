#include "Commons.hlsli"

struct SamplingVertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
    SamplingPixelShaderInput output;
    
    output.position = float4(input.position, 1.0);
    output.uv = input.uv;

    return output;
}
