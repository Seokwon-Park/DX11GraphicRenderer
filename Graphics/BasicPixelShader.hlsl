#include "Commons.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
    float3 rimColor;
    float rimPower; 
    float rimStrength = 0.0f; 
    bool useSmoothstep = false; 
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;
    
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-for
    // https://forum.unity.com/threads/what-are-unroll-and-loop-when-to-use-them.1283096/
    
    //i = 0
    [unroll] // warning X3557: loop only executes for 1 iteration(s), forcing loop to unroll
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, input.normalWorld, toEye);
    }
    
    // i =1
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    
    //i = 2
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    color += (1.f - dot(input.normalWorld, toEye)) * rimPower * rimStrength;

    return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.uv) : float4(color, 1.0);

}

