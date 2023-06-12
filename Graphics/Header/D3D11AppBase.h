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

		float GetAspectRatio() const;

		// ����
		int Run();

		// �ʱ�ȭ
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

	protected: // ��ӹ��� Ŭ���������� ������ ������ �Լ���=protected
		bool InitMainWindow();
		bool InitDirect3D11();
		bool InitGUI();


	public:
		int m_screenWidth;
		int m_screenHeight;
		HWND m_mainWindow;

		// ����̽�, ���ؽ�Ʈ
		ComPtr<ID3D11Device> m_d3dDevice;
		ComPtr<ID3D11DeviceContext> m_d3dContext;

		// ����Ÿ�� ��(RTV)
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		// ����ü��
		ComPtr<IDXGISwapChain> m_SwapChain;

		// �����Ͷ�����
		ComPtr<ID3D11RasterizerState> m_RasterizerState;
	};
}