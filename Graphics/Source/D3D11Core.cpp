#include "D3D11Core.h"

#include <directxtk/DDSTextureLoader.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace graphics
{
	namespace
	{
		// RegisterClassEx()에서 멤버 함수를 직접 등록할 수가 없기 때문에
		// AppBass 클래스의 MsgProc함수를 이용하여 간접적으로 메시지를 처리
		D3D11Core* g_appBase = nullptr;

		// 렌더링 윈도우 너비 높이 설정
		const int WIDTH = 1280;
		const int HEIHGT = 960;

		// RegisterClassEX()에 실제로 등록될 콜백함수
		LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			// g_appBase를 이용해서 간접적으로 멤버 함수 호출
			return g_appBase->MsgProc(hwnd, msg, wParam, lParam);
		}

		void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
			if (FAILED(hr)) {
				// 파일이 없을 경우
				if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
					std::cout << "File not found.\n";
				}

				// 에러 메시지가 있으면 출력
				if (errorBlob) {
					std::cout << "Shader compile error\n" << (char*)errorBlob->GetBufferPointer() << "\n";
				}
			}
		}
	}//anonymous namespace

	//Constructor
	D3D11Core::D3D11Core()
		:m_screenWidth(WIDTH), m_screenHeight(HEIHGT), m_mainWindow(0), m_screenViewport(D3D11_VIEWPORT())
	{
		g_appBase = this;
	}

	//Destructor
	D3D11Core::~D3D11Core()
	{
		g_appBase = nullptr;

		//Clean Up ImGui
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		//Destroy Window
		DestroyWindow(m_mainWindow);
	}

	float D3D11Core::GetAspectRatio() const
	{
		return float(m_screenWidth - m_guiWidth) / m_screenHeight;
	}

	bool show_demo_window = true;

	// 렌더러 실행 시
	int D3D11Core::Run()
	{
		// Main message loop
		MSG msg = { 0 };
		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
				ImGui_ImplWin32_NewFrame();


				//static bool test = true;
				ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
				ImGuizmo::BeginFrame();

				UpdateGUI(); // 추가적으로 사용할 GUI
				/*if (show_demo_window)
				{
					ImGui::ShowDemoWindow(&show_demo_window);

				}*/

				// IMGUI
				//auto size = ImGui::GetWindowSize();

				//ImGui::SetWindowPos(ImVec2(0.f, 0.f));
				m_guiWidth = 0.f;


				ImGui::Render(); // 렌더링할 것들 기록 끝

				// DX11
				Update(ImGui::GetIO().DeltaTime); // 애니메이션 같은 변화

				Render(); // 우리가 구현한 렌더링
				// DX11 END

				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링

				  // Update and Render additional Platform Windows
				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}

				// Switch the back buffer and the front buffer
				// 주의: ImGui RenderDrawData() 다음에 Present() 호출
				m_swapChain->Present(1, 0);
			}
		}

		return 0;
	}

	// Initialize Essentials(if failed, return false)
	bool D3D11Core::Initialize()
	{
		// Check Window Init
		if (!InitMainWindow())
		{
			return false;
		}

		// Check DirectX11 Init
		if (!InitDirect3D11())
		{
			return false;
		}

		// Check IMGUI Init
		if (!InitGUI())
		{
			return false;
		}

		// 콘솔창이 렌더링 창을 덮는 것 방지
		SetForegroundWindow(m_mainWindow);

		return true;
	}

	LRESULT D3D11Core::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;

		switch (msg) {
		case WM_SIZE:
			// Reset and resize swapchain
			if (m_swapChain) // 처음 실행인 경우
			{
				m_screenWidth = int(LOWORD(lParam));
				m_screenHeight = int(HIWORD(lParam));
				m_guiWidth = 0;

				m_renderTargetView.Reset();
				m_swapChain->ResizeBuffers(0, // 현재 개수 유지
					(UINT)LOWORD(lParam), // 해상도 변경
					(UINT)HIWORD(lParam),
					DXGI_FORMAT_UNKNOWN, // 현재 포멧
					0);
				CreateRenderTargetView();
				CreateDepthBuffer();
				SetViewport();
			}

			break;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_MOUSEMOVE:
			//std::cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << std::endl;
			if(m_useFPV)
				OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_LBUTTONUP:
			// cout << "WM_LBUTTONUP Left mouse button" << endl;
			break;
		case WM_RBUTTONUP:
			// cout << "WM_RBUTTONUP Right mouse button" << endl;
			break;
		case WM_KEYDOWN:
			std::cout << "WM_KEYDOWN " << (int)wParam << std::endl;

			m_keyPressed[wParam] = true;

			if (wParam == 27) {
				// ESC 키가 눌렸을 때 프로그램 종료
				DestroyWindow(hwnd);
			}
			break;
		case WM_KEYUP:
			m_keyPressed[wParam] = false;

			if (wParam == 70) { // 'f' 키
				m_useFPV = !m_useFPV;
			}
			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void D3D11Core::OnMouseMove(WPARAM btnState, int mouseX, int mouseY) {

		// 마우스 커서의 위치를 NDC로 변환
		// 마우스 커서는 좌측 상단 (0, 0), 우측 하단(width-1, height-1)
		// NDC는 좌측 하단이 (-1, -1), 우측 상단(1, 1)
		float x = mouseX * 2.0f / m_screenWidth - 1.0f;
		float y = -mouseY * 2.0f / m_screenHeight + 1.0f;

		// 커서가 화면 밖으로 나갔을 경우 범위 조절
		// 게임에서는 클램프를 안할 수도 있다.
		x = std::clamp(x, -1.0f, 1.0f);
		y = std::clamp(y, -1.0f, 1.0f);

		// 카메라 시점 회전
		m_camera.UpdateMouse(x, y);
	}

	bool D3D11Core::InitMainWindow()
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX),
					CS_CLASSDC,
					WndProc,
					0L,
					0L,
					GetModuleHandle(NULL),
					NULL,
					NULL,
					NULL,
					NULL,
					L"DX11GraphicsRenderer", // lpszClassName, L-string
					NULL };

		// The RegisterClass function has been superseded by the RegisterClassEx function.
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa?redirectedfrom=MSDN
		if (!RegisterClassEx(&wc)) {
			std::cout << "RegisterClassEx() failed.\n";
			return false;
		}

		// 원하는 크기의 해상도(툴바 제외)
		RECT windowRect = { 0, 0, m_screenWidth, m_screenHeight };

		// 필요한 윈도우 크기(해상도) 계산 (툴바를 포함)
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

		// 윈도우를 만들때 위에서 계산한 windowRect 사용
		m_mainWindow = CreateWindow(
			wc.lpszClassName,
			L"D3D11Renderer", 
			WS_OVERLAPPEDWINDOW,
			//윈도우의 생성 위치
			100,                // 윈도우 좌측 상단의 x 좌표
			100,                // 윈도우 좌측 상단의 y 좌표

			// 툴바를 포함한 실제 윈도우의 크기 계산
			windowRect.right - windowRect.left, // 윈도우 가로 방향 해상도
			windowRect.bottom - windowRect.top, // 윈도우 세로 방향 해상도
			NULL, NULL, wc.hInstance, NULL);

		if (!m_mainWindow) {
			std::cout << "CreateWindow() failed.\n";
			return false;
		}

		// 윈도우 표시
		ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
		UpdateWindow(m_mainWindow);

		return true;
	}

	bool D3D11Core::InitDirect3D11()
	{
		// 1. m_d3dDevice, m_d3dContext, 
		// 2. m_swapChain,
		// 3. m_renderTargetView,
		// 4. m_screenViewport,
		// 5. m_rasterizerState
		// 6. m_depthStencilState

		// 1. m_d3dDevice와 m_d3dContext 생성
		// 2. 내부에서 m_SwapChain도 같이 생성
		if(!CreateDeviceAndContext())
			return false;
		
		// 3. CreateRenderTargetView m_renderTargetView 생성
		if(!CreateRenderTargetView())
			return false;

		// 4. CreateViewport m_screenViewport 생성
		SetViewport();

		// 5. Create a solid rasterizer state
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthClipEnable = true;

		m_d3dDevice->CreateRasterizerState(&rastDesc, m_solidRasterizerState.GetAddressOf());

		// Create a wired rasterizer state
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

		m_d3dDevice->CreateRasterizerState(&rastDesc, m_wiredRasterizerState.GetAddressOf());

		// Create depth buffer
		CreateDepthBuffer();

		// Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthStencilDesc.DepthEnable = true; // false
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		if (FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc,
			m_depthStencilState.GetAddressOf()))) {
			std::cout << "CreateDepthStencilState() failed.\n";
			return false;
		}

		return true;
	}
	bool D3D11Core::InitGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;

			style.WindowPadding = ImVec2(10, 10);
			style.WindowRounding = 3.0f;
			style.FramePadding = ImVec2(5, 5);
			style.FrameRounding = 3.0f;
			style.ItemSpacing = ImVec2(12, 8);
			style.ItemInnerSpacing = ImVec2(8, 6);
			style.IndentSpacing = 15.0f;
			style.ScrollbarSize = 15.0f;
			style.ScrollbarRounding = 3.0f;
			style.GrabMinSize = 10.0f;
			style.GrabRounding = 2.0f;
				 
			style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(.2f, .2f, .2f, 1.00f);
			//style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			//style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			//style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			//style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			//style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
			//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
			//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
			//style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
		}

		if (!ImGui_ImplWin32_Init(m_mainWindow)) {
			return false;
		}

		// Setup Platform/Renderer backends
		if (!ImGui_ImplDX11_Init(m_d3dDevice.Get(), m_d3dContext.Get())) {
			return false;
		}

		return true;
	}


	bool D3D11Core::CreateDeviceAndContext()
	{
		// 그래픽카드 사용하도록 설정
		const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;

		const D3D_FEATURE_LEVEL featureLevels[3] = {
			D3D_FEATURE_LEVEL_11_1, // 더 높은 버전이 먼저 오도록 설정
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_9_3 };

		D3D_FEATURE_LEVEL featureLevel;

		if (FAILED(D3D11CreateDevice(
			nullptr,					// Specify nullptr to use the default adapter.
			driverType,					// Create a device using the hardware graphics driver.
			0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			createDeviceFlags,			// Set debug and Direct2D compatibility flags.
			featureLevels,				// List of feature levels this app can support.
			ARRAYSIZE(featureLevels),	// Size of the list above.
			D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
			device.GetAddressOf(),      // Returns the Direct3D device created.
			&featureLevel,				// Returns feature level of device created.
			context.GetAddressOf()      // Returns the device immediate context.
		))) {
			std::cout << "D3D11CreateDevice() failed.\n";
			return false;
		}

		//DX11을 지원하는지 확인
		if (!(featureLevel == D3D_FEATURE_LEVEL_11_1 || featureLevel == D3D_FEATURE_LEVEL_11_0)) {
			std::cout << "D3D Feature Level 11 unsupported.\n";
			return false;
		}

		// 참고: Immediate vs deferred context
		// A deferred context is primarily used for multithreading and is not necessary for a
		// single-threaded application.
		// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-intro#deferred-context

		// MultiSample Anti-Aliasing
		// 4X MSAA 지원하는지 확인
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_numQualityLevels);
		if (m_numQualityLevels <= 0) {
			std::cout << "MSAA not supported.\n";
		}
		// m_numQualityLevels = 0 // MSAA 끄기

		DXGI_SWAP_CHAIN_DESC sd = CreateSwapChainDesc();

		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
			0, // Default adapter
			driverType,
			0, // No software device
			createDeviceFlags,
			featureLevels,
			1,
			D3D11_SDK_VERSION,
			&sd,
			m_swapChain.GetAddressOf(),
			m_d3dDevice.GetAddressOf()
			, &featureLevel,
			m_d3dContext.GetAddressOf()));

		return true;
	}


	DXGI_SWAP_CHAIN_DESC D3D11Core::CreateSwapChainDesc()
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = m_screenWidth;               // set the back buffer width
		sd.BufferDesc.Height = m_screenHeight;             // set the back buffer height
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
		sd.BufferCount = 2;                                // Double-buffering
		sd.BufferDesc.RefreshRate.Numerator = 60;          // FPS
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
		sd.OutputWindow = m_mainWindow;                    // the window to be used
		sd.Windowed = TRUE;                                // windowed/full-screen mode
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		//MSAA 사용하는지 안하는지 확인
		if (m_numQualityLevels > 0) {
			sd.SampleDesc.Count = 4; // how many multisamples
			sd.SampleDesc.Quality = m_numQualityLevels - 1;
		}
		else {
			sd.SampleDesc.Count = 1; // how many multisamples
			sd.SampleDesc.Quality = 0;
		}

		return sd;
	}


	bool D3D11Core::CreateRenderTargetView()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		if (backBuffer) {
			m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), NULL,
				m_renderTargetView.GetAddressOf());
			m_d3dDevice->CreateShaderResourceView(backBuffer.Get(), NULL,
				m_shaderResourceView.GetAddressOf());
		}
		else {
			std::cout << "CreateRenderTargetView() failed." << std::endl;
			return false;
		}
		return true;
	}

	void D3D11Core::SetViewport()
	{
		//초기값을 0보다 작게 설정
		static float previousGuiWidth = -1.f;
		if (previousGuiWidth != m_guiWidth)
		{
			previousGuiWidth = m_guiWidth;

			// Set the viewport
			ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
			m_screenViewport.TopLeftX = float(m_guiWidth);
			m_screenViewport.TopLeftY = 0;
			m_screenViewport.Width = float(m_screenWidth - m_guiWidth);
			m_screenViewport.Height = float(m_screenHeight);
			//m_screenViewport.Width = static_cast<float>(m_screenHeight);
			m_screenViewport.MinDepth = 0.0f;
			m_screenViewport.MaxDepth = 1.0f;

			m_d3dContext->RSSetViewports(1, &m_screenViewport);
		}
	}

	bool D3D11Core::CreateDepthBuffer()
	{
		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		depthStencilBufferDesc.Width = m_screenWidth;
		depthStencilBufferDesc.Height = m_screenHeight;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (m_numQualityLevels > 0) {
			depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
			depthStencilBufferDesc.SampleDesc.Quality = m_numQualityLevels - 1;
		}
		else {
			depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
			depthStencilBufferDesc.SampleDesc.Quality = 0;
		}
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags = 0;

		if (FAILED(m_d3dDevice->CreateTexture2D(
			&depthStencilBufferDesc, 0, m_depthStencilBuffer.GetAddressOf()))) {
			std::cout << "CreateTexture2D() failed." << std::endl;
		}
		if (FAILED(m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0,
			&m_depthStencilView))) {
			std::cout << "CreateDepthStencilView() failed." << std::endl;
		}
		return true;
	}

	void D3D11Core::CreateDDSTexture(
		ComPtr<ID3D11Device>& device,
		const wchar_t* filename,
		ComPtr<ID3D11ShaderResourceView>& srv)
	{
		ComPtr<ID3D11Texture2D> texture;

		ThrowIfFailed(CreateDDSTextureFromFileEx(
			device.Get(), filename, 0, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false),
			(ID3D11Resource**)texture.GetAddressOf(),
			srv.GetAddressOf(), nullptr));
	}
}