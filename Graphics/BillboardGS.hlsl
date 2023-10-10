// Geometry-Shader Object
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-geometry-shader

// Stream-Output Object
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-so-type

cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix model; // For vertex shader
    Matrix view; // For vertex shader
    Matrix proj; // For vertex shader
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION; // not POSITION
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

//HINT: PointStream -> TriangleStream
[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    
    float hw = 0.5 * width;
    
    float4 up = float4(0.f, 1.f, 0.f, 0.f);
    float4 front = float4(eyeWorld, 1.f) - input[0].pos;
    front.w = 0.f;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.f);
    
    PixelShaderInput output;
    
    //output.pos = input[0].pos;
    
    //for (int i = 0; i < 100; i++)
    //{
    
    //    output.pos = input[0].pos + float4(0, 0.003, 0.0, 0.0) * float(i);
    //    output.pos = mul(output.pos, view);
    //    output.pos = mul(output.pos, proj);
    //    output.primID = primID;

    //    outputStream.Append(output);
    //}
    
    output.pos = input[0].pos - hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.uv = float2(0.f, 1.f);
    output.primID = primID;

    outputStream.Append(output);
    
    output.pos = input[0].pos - hw *right + hw* up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.uv = float2(0.f, 0.f);
    output.primID = primID;

    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.uv = float2(1.f, 1.f);
    output.primID = primID;

    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right + hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.uv = float2(1.f, 0.f);
    output.primID = primID;

    outputStream.Append(output);
    
   
    // 주의: GS는 Triangle Strips으로 출력합니다.
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/triangle-strips

    // outputStream.RestartStrip(); // Strip을 다시 시작
}
