#pragma once

#include "D3D11CommonHeaders.h"
#include "CommonHeaders.h"

namespace graphics
{
	using namespace Microsoft::WRL;

	class D3D11EngineBase
	{
	public:
		// Constructor & Destructor
		D3D11EngineBase();
		virtual ~D3D11EngineBase();

		// Get AspectRatio
		float GetAspectRatio() const;

		// Run Renderer
		int Run();

		// Initialize
		virtual bool Initialize();

		// Loop
		virtual void UpdateGUI() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		// MsgProc
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Mouse Events
		virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
		virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
		virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

	// ��ӹ��� Ŭ���������� ������ ������ �Լ��� = protected
	protected: 
		// Initialize Rendering Base
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