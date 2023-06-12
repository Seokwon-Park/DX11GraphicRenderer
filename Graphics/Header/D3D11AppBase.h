#pragma once

#include "CommonHeaders.h"

namespace graphics
{
	using namespace Microsoft::WRL;

	class D3D11AppBase
	{
	public:
		// 생성자
		D3D11AppBase();

		// 소멸자
		virtual ~D3D11AppBase();

		float GetAspectRatio() const;

		// 실행
		int Run();

		// 초기화
		virtual bool Initialize();
		virtual void UpdataGUI() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		// MsgProc
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Mouse Events
		virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
		virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
		virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

	protected: // 상속받은 클래스에서도 접근이 가능한 함수들=protected
		bool InitMainWindow();
		bool InitDirect3D11();
		bool InitGUI();


	public:
		int m_screenWidth;
		int m_screenHeight;
		HWND m_mainWindow;

		// 디바이스, 컨텍스트
		ComPtr<ID3D11Device> m_d3dDevice;
		ComPtr<ID3D11DeviceContext> m_d3dContext;

		// 렌더타겟 뷰(RTV)
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		// 스왑체인
		ComPtr<IDXGISwapChain> m_SwapChain;

		// 래스터라이저
		ComPtr<ID3D11RasterizerState> m_RasterizerState;
	};
}