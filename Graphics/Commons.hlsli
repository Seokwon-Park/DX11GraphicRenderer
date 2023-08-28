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
    float3 fresnelR0;
    float dummy3;
    float3 edgeTint;
    float dummy4;
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

// VSInput
struct VertexShaderInput
{
    float3 position : POSITION; //�� ��ǥ���� ��ġ position
    float3 normal : NORMAL; // �� ��ǥ���� normal    
    float2 uv : TEXCOORD0; // �� ��ǥ���� texcoord
    
    // float3 color : COLOR0; 
};

// PSInput(VSOutput)
struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posModel : POSITION0; // Model position
    float3 posWorld : POSITION1; // World position (���� ��꿡 ���)
    float3 normalWorld : NORMAL;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    
    // float3 color : COLOR; <- ���ʿ� (���̵�)
};


float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
                   float3 toEye, Material mat)
{
        float3 halfway = normalize(toEye + lightVec);
        float hdotn = dot(halfway, normal);
        float3 specular = mat.specular * pow(max(hdotn, 0.0), mat.shininess);
    
        return mat.ambient + (mat.diffuse + specular) * lightStrength;

        //float3 r = -reflect(lightVec, normal);
        //float3 specular = mat.specular * pow(max(dot(toEye, r), 0.0f), mat.shininess);
        //return mat.ambient + (mat.diffuse + specular) * lightStrength;

}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal,
                                float3 toEye)
{
    float3 lightVec = -L.direction;
    
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal,
                          float3 toEye)
{
    float3 lightVec = L.position - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);
    // ���̶� ������ �������
       

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d;
        
        float ndotl = max(dot(lightVec, normal), 0.0);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;

        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal,
                         float3 toEye)
{
    float3 lightVec = L.position - pos;

    // ���̵��� �������� ��������� �Ÿ� ���
    float d = length(lightVec);

    // �ʹ� �ָ� ������ ������� ����
    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= d;
        float ndotl = max(dot(lightVec, normal), 0.0);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        float spot = pow(max(-dot(lightVec, L.direction), 0.0), L.spotPower);
        lightStrength *= spot;
        
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
    
    // if�� else�� ���� ��� ��� �߻�
    // warning X4000: use of potentially uninitialized variable
}
