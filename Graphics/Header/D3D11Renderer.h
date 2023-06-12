#pragma once
#include "D3D11AppBase.h"

namespace graphics
{
	class D3D11Renderer : public D3D11AppBase
	{
	public:
		D3D11Renderer();

		virtual bool Initialize() override;
		virtual void UpdataGUI() override;
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
	};
}