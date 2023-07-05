// ���̴����� include�� ������� .hlsli ���Ͽ� �ۼ�
// Properties -> Item Type: Does not participate in build���� ����

// BlinnPhong ������ ��ü ������ Luna DX12 ����� ��������� 
// ���� ������ �����ϱ� ���ϵ��� ���� ���� ��Ÿ�Ϸ� �ܼ�ȭ�Ͽ����ϴ�.

/* ����: C++ SimpleMath -> HLSL */
// Matrix -> matrix �Ǵ� float4x4
// Vector3 -> float3
// float3 a = normalize(b);
// float a = dot(v1, v2);
// Satuarate() -> saturate() ���
// float l = length(v);
// struct A{ float a = 1.0f; }; <- ����ü �ȿ��� �ʱ�ȭ �Ұ�
// Vector3(0.0f) -> float3(0.0, 0.0, 0.0) // �Ǽ� �ڿ� f ���ʿ�
// Vector4::Transform(v, M) -> mul(v, M)

#define MAX_LIGHTS 3 // ���̴������� #define ��� ����
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

// ����
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1; // 16 bytes �����ֱ� ���� �߰�
    float3 specular;
    float dummy2;
};

// ����
struct Light
{
    float3 position;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 strength;
    float spotPower;
};

struct VertexShaderInput
{
    float3 position : POSITION; //�� ��ǥ���� ��ġ position
    float3 normal : NORMAL; // �� ��ǥ���� normal    
    float2 uv : TEXCOORD0; // <- ���� �������� ���
    
    // float3 color : COLOR0; <- ���ʿ� (���̵�)
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (���� ��꿡 ���)
    float3 normalWorld : NORMAL;
    float2 uv : TEXCOORD;
    
    // float3 color : COLOR; <- ���ʿ� (���̵�)
};
