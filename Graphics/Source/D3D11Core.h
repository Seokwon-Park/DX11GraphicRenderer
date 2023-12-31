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

		// RenderTargetView에서 원본을 여기에 그림
		ComPtr<ID3D11Texture2D> m_tempTexture;
		// indexRenderTargetView가 그리는 indexColor로 표시된 이미지를 여기에 그림.
		ComPtr<ID3D11Texture2D> m_indexTexture;

		// indexTexture를 복사해서 가져옴
		ComPtr<ID3D11Texture2D> m_indexTempTexture;
		// indexTempTexture에서 1x1크기의 픽셀을 추출한다.
		ComPtr<ID3D11Texture2D> m_indexStagingTexture;

		// 4. RasterizerState
		ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
		ComPtr<ID3D11RasterizerState> m_wiredRasterizerState;
		bool m_drawAsWire = false;
		bool m_usePostProcessing = true;

		// Depth buffer 관련
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		
		// 5. Viewport
		D3D11_VIEWPORT m_screenViewport;

		// 아직 동작안함.
		D3D11_VIEWPORT m_screenViewport2;

		// Camera
		D3D11Camera m_camera;
		// First Person View
		bool m_useFPV = false;

		// Keyboard Event
		bool m_keyPressed[256] = {
			false,
		};

		// MouseEvent
		bool m_leftButton = false;
		bool m_rightButton = false;
		bool m_dragStartFlag = false;
		bool m_selected = false;

		// Mouse Position
		int m_cursorX = 0;
		int m_cursorY = 0;
		float m_cursorNdcX = 0.0f;
		float m_cursorNdcY = 0.0f;
	};
}