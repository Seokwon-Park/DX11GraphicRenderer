#include "Commons.hlsli"

cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
    // 불필요한 멤버들도 VertexShaderInput을 통일시켰기 때문에 채워줘야 합니다.
    
    PixelShaderInput output;
    output.posModel = input.position;
    float4 pos = float4(input.position, 1.0f);

    pos = mul(pos, model); // Identity

    output.posWorld = pos.xyz;
    
    float4 normal = float4(input.normal, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);

    pos = mul(pos, view);
    
    pos = mul(pos, projection);
    output.posProj = pos;

    output.uv = input.uv;
    output.color = float3(1.0, 1.0, 0.0);

    return output;
}
