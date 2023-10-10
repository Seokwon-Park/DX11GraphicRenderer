Texture2DArray g_texArray : register(t0);
SamplerState g_sampler : register(s0);

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    float3 uvw = float3(input.uv, float(input.primID % 5));
    float4 pixelColor = g_texArray.Sample(g_sampler, uvw);
    
    clip((pixelColor.a < .9f) || (pixelColor.r + pixelColor.g + pixelColor.b) > 2.0f ? -1 : 1);
    
    PixelShaderOutput output;
    
    output.pixelColor = pixelColor;
    //output.pixelColor = (input.primID % 2 == 0) ?
    //    float4(1.0f, 1.0f, 1.0f, 1.0f) : float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}
