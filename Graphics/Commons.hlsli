// 쉐이더에서 include할 내용들은 .hlsli 파일에 작성
// Properties -> Item Type: Does not participate in build으로 설정

// BlinnPhong 구현의 전체 구조는 Luna DX12 교재와 비슷하지만 
// 세부 구현은 이해하기 편하도록 대학 강의 스타일로 단순화하였습니다.

/* 참고: C++ SimpleMath -> HLSL */
// Matrix -> matrix 또는 float4x4
// Vector3 -> float3
// float3 a = normalize(b);
// float a = dot(v1, v2);
// Satuarate() -> saturate() 사용
// float l = length(v);
// struct A{ float a = 1.0f; }; <- 구조체 안에서 초기화 불가
// Vector3(0.0f) -> float3(0.0, 0.0, 0.0) // 실수 뒤에 f 불필요
// Vector4::Transform(v, M) -> mul(v, M)

#define MAX_LIGHTS 3 // 쉐이더에서도 #define 사용 가능
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

// 재질
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1; // 16 bytes 맞춰주기 위해 추가
    float3 specular;
    float dummy2;
    float3 fresnelR0;
    float dummy3;
    float3 edgeTint;
    float dummy4;
};

// 조명
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
    float3 position : POSITION; //모델 좌표계의 위치 position
    float3 normal : NORMAL; // 모델 좌표계의 normal    
    float2 uv : TEXCOORD0; // 모델 좌표계의 texcoord
    
    // float3 color : COLOR0; 
};

// PSInput(VSOutput)
struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posModel : POSITION0; // Model position
    float3 posWorld : POSITION1; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    
    // float3 color : COLOR; <- 불필요 (쉐이딩)
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

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);
    // 길이라서 방향은 상관없음
       

    // 너무 멀면 조명이 적용되지 않음
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

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);

    // 너무 멀면 조명이 적용되지 않음
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
    
    // if에 else가 없을 경우 경고 발생
    // warning X4000: use of potentially uninitialized variable
}
