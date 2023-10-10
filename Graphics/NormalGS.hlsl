cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix world;
    matrix invTranspose;
    matrix view;
    matrix projection;
}

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL;
};

struct PixelShaderInput
{
	float4 pos: SV_POSITION;
    float3 color : COLOR;
};

[maxvertexcount(2)]
void main(
	point GeometryShaderInput input[1],
	inout LineStream<PixelShaderInput> outputStream
)
{
    PixelShaderInput output;
    
    float4 posWorld = mul(input[0].posModel, world);
    float4 normalModel = float4(input[0].normalModel, 0.f);
    float4 normalWorld = mul(normalModel, invTranspose);
    normalWorld = float4(normalize(normalWorld.xyz), 0.f);
    
    output.pos= mul(posWorld, view);
    output.pos= mul(output.pos, projection);
    output.color = float3(0.f, 1.f, 0.f);
    outputStream.Append(output);
        
    output.pos = mul(posWorld + .1f * normalWorld, view);
    output.pos = mul(output.pos, projection);
    output.color = float3(1.f, 1.f, 0.f);
    outputStream.Append(output);
}