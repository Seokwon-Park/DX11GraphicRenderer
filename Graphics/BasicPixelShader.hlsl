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
    float4 indexColor;
    float rimStrength;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
    float4 indexColor : SV_Target1;
};

//artist friendly metalic Fresnel
//http://jcgt.org/published/0003/04/03/paper-lowres.pdf
float3 SchlickFresnel(float3 fresnelR0, float3 edgeTint, float theta)
{
    //clamp parameters
    float3 r = clamp(fresnelR0, 0, 0.99);
    float3 g = edgeTint;
    float3 r_sqrt = sqrt(r);
    float3 n_min = (1.0 - r) / (1.0 + r);
    float3 n_max = (1.0 + r_sqrt) / (1.0 - r_sqrt);
    float3 n = lerp(n_max, n_min, g);
    float3 k2 = ((n + 1.0) * (n + 1.0) * r - (n - 1.0) * (n - 1.0)) / (1.0 - r);
    k2 = max(k2, 0.0);
    float3 k = sqrt(k2);
//compute n and k
       
    float3 rs_num = n * n + k2 - 2.f * n * theta + theta * theta;
    float3 rs_den = n * n + k2 + 2.f * n * theta + theta * theta;
    float3 rs = rs_num / rs_den;
    
    float3 rp_num = (n * n + k2) * theta * theta - 2 * n * theta + 1;
    float3 rp_den = (n * n + k2) * theta * theta + 2 * n * theta + 1;
    float3 rp = rp_num / rp_den;
    return clamp(0.5f * (rs + rp), 0.f, 1.f);
}

float3 SchlickFresnel2(float3 fresnelR0, float3 normal, float3 toEye)
{
    // 참고 자료들
    // THE SCHLICK FRESNEL APPROXIMATION by Zander Majercik, NVIDIA
    // http://psgraphics.blogspot.com/2020/03/fresnel-equations-schlick-approximation.html
    
    float normalDotView = saturate(dot(normal, toEye));

    float f0 = 1.0f - normalDotView; // 90도이면 f0 = 1, 0도이면 f0 = 0

    // 1.0 보다 작은 값은 여러 번 곱하면 더 작은 값이 됩니다.
    // 0도 -> f0 = 0 -> fresnelR0 반환
    // 90도 -> f0 = 1.0 -> float3(1.0) 반환
    // 0도에 가까운 가장자리는 Specular 색상, 90도에 가까운 안쪽은 고유 색상(fresnelR0)
    return fresnelR0 + (1.0f - fresnelR0) * pow(f0, 5.0);
}

PixelShaderOutput main(PixelShaderInput input)
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
    
    //if (useRim)
    //{
    //    float rim = (1.f - dot(input.normalWorld, toEye));
    
    //    if (useSmoothstep)
    //        rim = smoothstep(0.f, 1.f, rim);
    
    //    rim = pow(abs(rim), rimPower);
        
    //    color += rim * rimColor * rimStrength;
    //}

    float3 reflectDir = reflect(-toEye, input.normalWorld);
    
    float4 diffuse = g_textureCube1.Sample(g_sampler, input.normalWorld);
    float4 specular = g_textureCube2.Sample(g_sampler, reflectDir);

    specular *= pow((specular.r + specular.g + specular.b) / 3.f, material.shininess);
    
    diffuse *= float4(material.diffuse, 1.f);
    specular *= float4(material.specular, 1.f);
    
    float theta = dot(toEye, input.normalWorld);
    
    float3 f = SchlickFresnel(material.fresnelR0, material.edgeTint, theta);
    specular.xyz *= f;
    
    //if (useRim)
    //{
    //    //float3 f = SchlickFresnel(material.fresnelR0, material.edgeTint, theta);
    //    //specular.xyz *= f;
        
    //    float3 f = SchlickFresnel2(material.fresnelR0, input.normalWorld, toEye);
    //    specular.xyz *= f;
    //}
    
    PixelShaderOutput output;
    output.pixelColor = diffuse + specular;
    output.indexColor = indexColor;
        
    output.pixelColor = useTexture ? diffuse * g_texture0.Sample(g_sampler, input.uv) + specular + diffuse : specular + diffuse;
    
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.uv) : float4(color, 1.0);
    return output;
}

