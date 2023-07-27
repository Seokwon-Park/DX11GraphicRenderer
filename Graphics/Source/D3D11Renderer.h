#pragma once
#include "D3D11AppBase.h"

namespace graphics
{

	using namespace DirectX;

	// Vertex���̴� ���
	struct BasicVertexConstantBuffer {
		XMMATRIX world;
		XMMATRIX invTranspose;
		XMMATRIX view;
		XMMATRIX projection;
	};

		static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

#define MAX_LIGHTS 3

	// ����
	struct Material {
		XMFLOAT3 ambient = XMFLOAT3(0.1f, 0.1f, 0.1f);  // 12byte
		float shininess = 1.0f;							// 4byte
		XMFLOAT3 diffuse = XMFLOAT3(0.5f, 0.5f, 0.5f);  // 12byte
		float pad1;										// 4byte
		XMFLOAT3 specular = XMFLOAT3(0.5f, 0.5f, .5f);	// 12byte
		float pod2;										// 4byte
	};

	// ����
	struct Light {
		// ������ ũ�� ���� ���� (16 ����Ʈ �е�)
		XMFLOAT3 position = XMFLOAT3(0.f, 0.f, -2.f);// 12byte
		float fallOffStart = 0.0f;						// 4byte
		XMFLOAT3 direction = XMFLOAT3(0.f, 0.f, 1.f);// 12byte
		float fallOffEnd = 10.0f;						// 4byte
		XMFLOAT3 strength = XMFLOAT3(1.f, 1.f, 1.f);	// 12byte
		float spotPower = 1.0f;							// 4byte
	};

	// Pixel���̴� ���
	struct BasicPixelConstantBuffer {
		XMFLOAT3 eyeWorld;        // 12
		bool useTexture;		  // 4
		Material material;		  // 48 16*3
		Light lights[MAX_LIGHTS]; // 48 * MAX_LIGHTS
	};

	static_assert((sizeof(BasicPixelConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

	struct NormalVertexConstantBuffer {
		float scale = 0.1f;
		float pad[3];
	};

	static_assert((sizeof(NormalVertexConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

	class D3D11Renderer : public D3D11AppBase
	{
	public:
		D3D11Renderer();

		virtual bool Initialize() override;
		virtual void UpdateGUI() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

	protected:
		std::shared_ptr<Mesh> m_mesh;

		//Shaders
		//Basic
		ComPtr<ID3D11VertexShader> m_colorVertexShader;
		ComPtr<ID3D11PixelShader> m_colorPixelShader;
		ComPtr<ID3D11InputLayout> m_colorInputLayout;

		//Normal
		ComPtr<ID3D11VertexShader> m_normalVertexShader;
		ComPtr<ID3D11PixelShader> m_normalPixelShader;

		// ConstantBufferData(Cpp) -> ConstantBuffer(Shader)
		BasicVertexConstantBuffer m_basicVertexConstantBufferData;
		BasicPixelConstantBuffer m_basicPixelConstantBufferData;

		//Normal
		NormalVertexConstantBuffer m_normalVertexConstantBufferData;

		// Textures
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
		ComPtr<ID3D11SamplerState> m_samplerState;

		// perspective vs orthographic
		bool m_usePerspectiveProjection = true;
		
		// Transforms
		XMFLOAT3 m_modelTranslation = XMFLOAT3();
		XMFLOAT3 m_modelRotation = XMFLOAT3();
		XMFLOAT3 m_modelScaling = XMFLOAT3(0.5f, 0.5f, 0.5f);

		//XMFLOAT3 m_viewEyePos= XMFLOAT3(0.0f,0.0f,-2.0f);
		//XMFLOAT3 m_viewEyeDir= XMFLOAT3(0.0f, 0.0f, 1.0f);
		//XMFLOAT3 m_viewUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
		float m_viewRot = 0.0f;

		//Frustum
		//Field of View
		float m_projFovAngleY = 70.0f;
		float m_nearZ = 0.01f;
		float m_farZ = 10.0f;
		float m_aspect = D3D11AppBase::GetAspectRatio();		

		//Light
		int m_lightType = 0;
		Light m_lightFromGUI;
		float m_materialDiffuse = .5f;
		float m_materialSpecular = .7f;
		
		//Normal
		std::shared_ptr<Mesh> m_normalLines;
		bool m_drawNormals = true;
		bool m_dirtyFlag = false;
	};
} // namespace graphics