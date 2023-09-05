#pragma once

#include "D3D11CommonHeaders.h"
#include "CommonHeaders.h"
#include "D3D11PostProcess.h"
#include "D3D11Camera.h"

namespace graphics
{
	using namespace Microsoft::WRL;

	class D3D11Core
	{
	public:
		// Constructor & Destructor
		D3D11Core();
		virtual ~D3D11Core();

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
		virtual void OnMouseMove(WPARAM btnState, int x, int y);

		static void CreateDDSTexture(
			ComPtr<ID3D11Device>& device,
			const wchar_t* filename,
			ComPtr<ID3D11ShaderResourceView>& srv);

	// 상속받은 클래스에서도 접근이 가능한 함수들 = protected
	protected: 
		// Initialize Rendering Base
		bool InitMainWindow();
		bool InitDirect3D11();
		bool InitGUI();

		bool CreateDeviceAndContext();
		DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc();
		bool CreateRenderTargetView();
		void SetViewport();
		bool CreateDepthBuffer();

	public:
		// MainWindow Variables
		int m_screenWidth;
		int m_screenHeight;
		float m_guiWidth = 0;
		HWND m_mainWindow;
		UINT m_numQualityLevels = 0;

		// Mouse Vars
		int m_cursorX;
		int m_cursorY;

		// D3D11 Base		
		// 1. 디바이스, 컨텍스트
		ComPtr<ID3D11Device> m_d3dDevice;
		ComPtr<ID3D11DeviceContext> m_d3dContext;
		// 2. Swapchain
		ComPtr<IDXGISwapChain> m_swapChain;
		// 3. 렌더타겟 뷰(RTV)
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		ComPtr<ID3D11RenderTargetView> m_indexRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		ComPtr<ID3D11Texture2D> m_tempTexture;
		ComPtr<ID3D11Texture2D> m_indexTexture;
		ComPtr<ID3D11Texture2D> m_indexTempTexture;
		ComPtr<ID3D11Texture2D> m_indexStagingTexture;

		// 4. RasterizerState
		ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
		ComPtr<ID3D11RasterizerState> m_wiredRasterizerState;
		bool m_drawAsWire = false;


		// Depth buffer 관련
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		
		
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		// 5. Viewport
		D3D11_VIEWPORT m_screenViewport;
		D3D11_VIEWPORT m_screenViewport2;
		D3D11Camera m_camera;

		bool m_keyPressed[256] = {
			false,
		};

		bool m_useFPV = false;
		bool m_leftButton = false;
	};
}