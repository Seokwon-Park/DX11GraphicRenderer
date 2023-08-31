#include "Commons.hlsli"

cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;   
};

cbuffer NormalVertexConstantBuffer : register(b1)
{
    //matrix model;
    //matrix invTranspose;
    //matrix view;
    //matrix projection;
    float scale; // 그려지는 선분의 길이 조절
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.position, 1.0f);

    // Normal 먼저 변환
    float4 normal = float4(input.normal, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    
    pos = mul(pos, model);
    
    float t = input.uv.x;
    
    pos.xyz += output.normalWorld * t * scale;

    //posModel
    output.posModel = input.position;
    
    //posWorld
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);

    ///uv
    output.posProj = pos;
    output.uv = input.uv;
    
    //color 
    output.color = lerp(float3(1.0, 1.0, 0.0), float3(1.0, 0.0, 0.0), t);

    return output;
}
