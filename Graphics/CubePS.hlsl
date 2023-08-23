#include "Commons.hlsli" 

TextureCube g_textureCube0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    // 주의: 텍스춰 좌표가 float3 입니다.
    return g_textureCube0.Sample(g_sampler, input.posWorld.xyz);
}