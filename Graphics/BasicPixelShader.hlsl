#include "Commons.hlsli"

Texture2D g_texture0 : register(t0);
TextureCube g_textureCube1 : register(t1);
TextureCube g_textureCube2 : register(t2);

SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
    float3 rimColor;
    float rimPower;
    float rimStrength;
    bool useSmoothstep;
    bool useRim;
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
    
    float2 uv;
    uv.x = atan2(input.posModel.z, input.posModel.x) / (3.141592 * 2.0) + 0.5;
    uv.y = acos(input.posModel.y / 1.5) / 3.141592;
    
    if (useRim)
    {
        float rim = (1.f - dot(input.normalWorld, toEye));
    
        if (useSmoothstep)
            rim = smoothstep(0.f, 1.f, rim);
    
        rim = pow(abs(rim), rimPower);
        
        color += rim * rimColor * rimStrength;
    }

    float3 coord = reflect(-toEye, input.normalWorld);
    
    float4 diffuse = g_textureCube1.Sample(g_sampler, input.normalWorld);
    float4 specular = g_textureCube2.Sample(g_sampler, coord);

    diffuse.xyz *= material.diffuse;
    specular *= pow((specular.x + specular.y + specular.z)/3.f, material.shininess);
    specular.xyz *= material.specular;
    
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.uv) : float4(color, 1.0);
    return useTexture ? g_texture0.Sample(g_sampler, input.uv) + diffuse + specular
                        : diffuse + specular;

}

