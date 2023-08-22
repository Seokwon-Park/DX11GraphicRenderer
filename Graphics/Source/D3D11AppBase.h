#pragma once

#include "CommonHeaders.h"


namespace graphics
{
	using namespace Microsoft::WRL;

	class D3D11AppBase
	{
	public:
		// ������
		D3D11AppBase();
		// �Ҹ���
		virtual ~D3D11AppBase();

		// ȭ�� ���κ�
		float GetAspectRatio() const;

		// ������ ���� ��
		int Run();

		// �ʱ�ȭ
		virtual bool Initialize();
		virtual void UpdateGUI() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		// MsgProc
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Mouse Events
		virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
		virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
		virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

	protected: // ��ӹ��� Ŭ���������� ������ ������ �Լ��� = protected
		bool InitMainWindow();
		bool InitDirect3D11();
		bool InitGUI();

		void SetViewport();
		bool CreateRenderTargetView();
		bool CreateDepthBuffer();

		void CreateVertexShaderAndInputLayout(
			const std::wstring& filename,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			ComPtr<ID3D11VertexShader>& vertexShader,
			ComPtr<ID3D11InputLayout>& inputLayout);
		void CreatePixelShader(const std::wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer);

		// Template�� ��� ���Ͽ� ����
		template <typename T_VERTEX>
		void CreateVertexBuffer(const std::vector<T_VERTEX>& vertices, ComPtr<ID3D11Buffer>& vertexBuffer) {

			// D3D11_USAGE enumeration (d3d11.h)
			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // �ʱ�ȭ �� ����X
			bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
			bufferDesc.StructureByteStride = sizeof(T_VERTEX);

			D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 }; // MS �������� �ʱ�ȭ�ϴ� ���
			vertexBufferData.pSysMem = vertices.data();
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr =
				m_d3dDevice->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
			if (FAILED(hr)) {
				std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
			};
		}

		template <typename T_CONSTANT>
		void CreateConstantBuffer(const T_CONSTANT& constantBufferData,
			ComPtr<ID3D11Buffer>& constantBuffer) {
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth = sizeof(constantBufferData);
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;

			// Fill in the subresource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = &constantBufferData;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			auto hr = m_d3dDevice->CreateBuffer(&cbDesc, &InitData, constantBuffer.GetAddressOf());

			if (FAILED(hr)) {
				std::cout << "CreateConstantBuffer() CreateBuffer failed()."
					<< std::endl;
			}
		}

		template <typename T_DATA>
		void UpdateBuffer(const T_DATA& bufferData, ComPtr<ID3D11Buffer>& buffer) {

			if (!buffer) {
				std::cout << "UpdateBuffer() buffer was not initialized."
					<< std::endl;
			}

			D3D11_MAPPED_SUBRESOURCE ms;
			m_d3dContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, &bufferData, sizeof(bufferData));
			m_d3dContext->Unmap(buffer.Get(), NULL);
		}


		void CreateTexture(const std::string filename,
			ComPtr<ID3D11Texture2D>	&texture,
			ComPtr<ID3D11ShaderResourceView> &textureResourceView);

	public:
		// MainWindow Variables
		int m_screenWidth;
		int m_screenHeight;
		float m_guiWidth = 0;
		HWND m_mainWindow;
		UINT m_numQualityLevels = 0;

		// D3D11 Base		
		// ����̽�, ���ؽ�Ʈ
		ComPtr<ID3D11Device> m_d3dDevice;
		ComPtr<ID3D11DeviceContext> m_d3dContext;
		// ����Ÿ�� ��(RTV)
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		// Swapchain
		ComPtr<IDXGISwapChain> m_swapChain;
		// RasterizerState
		ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
		ComPtr<ID3D11RasterizerState> m_wiredRasterizerState;
		bool m_drawAsWire = false;

		// Depth buffer ����
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		ComPtr<ID3D11DepthStencilState> m_depthStencilState;

		D3D11_VIEWPORT m_screenViewport;
	};
}