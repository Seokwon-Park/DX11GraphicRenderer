#include "Commons.hlsli" 

TextureCube g_textureCube0 : register(t0);
TextureCube g_textureCube1 : register(t1);
//Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    //float2 uv;
    //uv.x = atan2(input.posModel.z, input.posModel.x) / (3.141592 * 2.0) + 0.5;
    //uv.y = acos(input.posModel.y / 1.5) / 3.141592;
    // 주의: 텍스춰 좌표가 float3 입니다.
    return g_textureCube1.Sample(g_sampler, input.posWorld.xyz);
    //return g_texture0.Sample(g_sampler, input.uv);
}