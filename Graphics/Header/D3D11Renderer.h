#pragma once
#include "D3D11AppBase.h"

namespace graphics
{

	using namespace DirectX;

	// 이 예제에서 ConstantBuffer로 보낼 데이터
	struct ModelViewProjectionConstantBuffer {
		XMMATRIX model = XMMATRIX();
		XMMATRIX view = XMMATRIX();
		XMMATRIX projection = XMMATRIX();
	};
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

		ComPtr<ID3D11VertexShader> m_colorVertexShader;
		ComPtr<ID3D11PixelShader> m_colorPixelShader;
		ComPtr<ID3D11InputLayout> m_colorInputLayout;

		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;
		ComPtr<ID3D11Buffer> m_constantBuffer;
		UINT m_indexCount;

		ModelViewProjectionConstantBuffer m_constantBufferData;

		bool m_usePerspectiveProjection = true;
		XMFLOAT3 m_modelTranslation = XMFLOAT3();
		XMFLOAT3 m_modelRotation = XMFLOAT3();
		XMFLOAT3 m_modelScaling = XMFLOAT3(0.5f, 0.5f, 0.5f);
		XMFLOAT3 m_viewEyePos= XMFLOAT3(0.0f,0.0f,-2.0f);
		XMFLOAT3 m_viewEyeDir= XMFLOAT3(0.0f, 0.0f, 1.0f);
		XMFLOAT3 m_viewUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
		float m_projFovAngleY = 70.0f;
		float m_nearZ = 0.01f;
		float m_farZ = 100.0f;
		float m_aspect = D3D11AppBase::GetAspectRatio();		
	};
} // namespace graphics