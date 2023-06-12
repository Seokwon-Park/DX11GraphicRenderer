#pragma once
#include "D3D11AppBase.h"

namespace graphics
{

	using namespace DirectX;
	// �� �������� ����ϴ� Vertex ���� 
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 color;
	};

	// �� �������� ConstantBuffer�� ���� ������
	struct ModelViewProjectionConstantBuffer {
		XMMATRIX model;
		XMMATRIX view;
		XMMATRIX projection;
	};
	class D3D11Renderer : public D3D11AppBase
	{
	public:
		D3D11Renderer();

		virtual bool Initialize() override;
		virtual void UpdateGUI() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

	private:
		ComPtr<ID3D11VertexShader> m_colorVertexShader;
		ComPtr<ID3D11PixelShader> m_colorPixelShader;
		ComPtr<ID3D11InputLayout> m_colorInputLayout;

		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;
		ComPtr<ID3D11Buffer> m_constantBuffer;
		UINT m_indexCount;

		ModelViewProjectionConstantBuffer m_constantBufferData;

		bool m_usePerspectiveProjection = true;
	};
}