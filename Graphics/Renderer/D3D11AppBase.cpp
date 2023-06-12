#include "D3D11AppBase.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);

namespace graphics
{
 	// RegisterClassEx()에서 멤버 함수를 직접 등록할 수가 없기 때문에
	// AppBass 클래스의 MsgProc함수를 이용하여 간접적으로 메시지를 처리할 수 있도록 한다.
	D3D11AppBase* g_appBase = nullptr;
	const int WIDTH = 1280;
	const int HEIHGT = 960;

	// RegisterClassEX()에 실제로 등록될 콜백함수
	LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// g_appBase를 이용해서 간접적으로 멤버 함수 호출
		return g_appBase->MsgProc(hwnd, msg, wParam, lParam);
	}

	D3D11AppBase::D3D11AppBase()
		:m_screenWidth(WIDTH), m_screenHeight(HEIHGT), m_mainWindow(0), m_screenViewport(D3D11_VIEWPORT())
	{
		g_appBase = this;
	}

	D3D11AppBase::~D3D11AppBase()
	{
		g_appBase = nullptr;

		//Clean Up ImGui
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		DestroyWindow(m_mainWindow);
	}

	float D3D11AppBase::GetAspectRatio() const
	{
		return float(m_screenWidth) / m_screenHeight;
	}

	// 렌더러 실행 시
	int D3D11AppBase::Run()
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

                ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
                ImGui::Begin("Scene Control");

                // ImGui가 측정해주는 Framerate 출력
                ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

                UpdateGUI(); // 추가적으로 사용할 GUI

                ImGui::End();
                ImGui::Render(); // 렌더링할 것들 기록 끝

                Update(ImGui::GetIO().DeltaTime); // 애니메이션 같은 변화

                Render(); // 우리가 구현한 렌더링

                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링

                // Switch the back buffer and the front buffer
                // 주의: ImGui RenderDrawData() 다음에 Present() 호출
                m_swapChain->Present(1, 0);
            }
        }

        return 0;
	}

	// 초기화
	bool D3D11AppBase::Initialize()
	{
		// Window 초기화 검사
		if (!InitMainWindow())
		{
			return false;
		}
		
		// DirectX11 초기화 검사
		if (!InitDirect3D11())
		{
			return false;
		}

		// IMGUI 초기화 검사
		if (!InitGUI())
		{
			return false;
		}

		return true;
	}

    LRESULT D3D11AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
            return true;

        switch (msg) {
        case WM_SIZE:
            // Reset and resize swapchain
            break;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_MOUSEMOVE:
            // cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << endl;
            break;
        case WM_LBUTTONUP:
            // cout << "WM_LBUTTONUP Left mouse button" << endl;
            break;
        case WM_RBUTTONUP:
            // cout << "WM_RBUTTONUP Right mouse button" << endl;
            break;
        case WM_KEYDOWN:
            // cout << "WM_KEYDOWN " << (int)wParam << endl;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }

        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

	bool D3D11AppBase::InitMainWindow()
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
					L"D3D11GraphicsRenderer", // lpszClassName, L-string
					NULL };

		// The RegisterClass function has been superseded by the RegisterClassEx function.
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa?redirectedfrom=MSDN
		if (!RegisterClassEx(&wc)) {
			std::cout << "RegisterClassEx() failed.\n";
			return false;
		}

		// 툴바까지 포함한 윈도우 전체 해상도가 아니라
		// 우리가 실제로 그리는 해상도가 width x height가 되도록
		// 윈도우를 만들 해상도를 다시 계산해서 CreateWindow()에서 사용

		// 우리가 원하는 그림이 그려질 부분의 해상도
		RECT windowRect = { 0, 0, m_screenWidth, m_screenHeight };

		// 필요한 윈도우 크기(해상도) 계산
		// windowRect의 값이 바뀜
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

		// 윈도우를 만들때 위에서 계산한 windowRect 사용
		m_mainWindow = CreateWindow(wc.lpszClassName, L"D3D11Renderer", WS_OVERLAPPEDWINDOW,
			100,                // 윈도우 좌측 상단의 x 좌표
			100,                // 윈도우 좌측 상단의 y 좌표
			windowRect.right - windowRect.left, // 윈도우 가로 방향 해상도
			windowRect.bottom - windowRect.top, // 윈도우 세로 방향 해상도
			NULL, NULL, wc.hInstance, NULL);

		if (!m_mainWindow) {
			std::cout << "CreateWindow() failed.\n";
			return false;
		}

		ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
		UpdateWindow(m_mainWindow);

		return true;
	}

	bool D3D11AppBase::InitDirect3D11()
	{
        const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

        // 여기서 생성하는 것들
        // m_d3dDevice, m_d3dContext, m_swapChain,
        // m_renderTargetView, m_screenViewport, m_rasterizerSate

        // m_d3dDevice와 m_d3dContext 생성
        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;

        const D3D_FEATURE_LEVEL featureLevels[3] = {
            D3D_FEATURE_LEVEL_11_1, // 더 높은 버전이 먼저 오도록 설정
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_9_3};

        D3D_FEATURE_LEVEL featureLevel;

        if (FAILED(D3D11CreateDevice(
            nullptr,                  // Specify nullptr to use the default adapter.
            driverType,               // Create a device using the hardware graphics driver.
            0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,        // Set debug and Direct2D compatibility flags.
            featureLevels,            // List of feature levels this app can support.
            ARRAYSIZE(featureLevels), // Size of the list above.
            D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
            &device,           // Returns the Direct3D device created.
            &featureLevel,     // Returns feature level of device created.
            &context           // Returns the device immediate context.
        ))) {
            std::cout << "D3D11CreateDevice() failed.\n";
            return false;
        }

        if (!(featureLevel == D3D_FEATURE_LEVEL_11_1 || featureLevel == D3D_FEATURE_LEVEL_11_0)) {
            std::cout << "D3D Feature Level 11 unsupported.\n";
            return false;
        }

        // 참고: Immediate vs deferred context
        // A deferred context is primarily used for multithreading and is not necessary for a
        // single-threaded application.
        // https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-intro#deferred-context

        // 4X MSAA 지원하는지 확인
        UINT numQualityLevels;
        device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
        if (numQualityLevels <= 0) {
            std::cout << "MSAA not supported.\n";
        }

        // numQualityLevels = 0 // MSAA 끄기

        // Swapchain Description
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width = m_screenWidth;               // set the back buffer width
        sd.BufferDesc.Height = m_screenHeight;             // set the back buffer height
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
        sd.BufferCount = 2;                                // Double-buffering
        sd.BufferDesc.RefreshRate.Numerator = 60;          // FPS인듯?
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
        sd.OutputWindow = m_mainWindow;                    // the window to be used
        sd.Windowed = TRUE;                                // windowed/full-screen mode
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        //MSAA 사용하는지 안하는지 확인
        if (numQualityLevels > 0) {
            sd.SampleDesc.Count = 4; // how many multisamples
            sd.SampleDesc.Quality = numQualityLevels - 1;
        }
        else {
            sd.SampleDesc.Count = 1; // how many multisamples
            sd.SampleDesc.Quality = 0;
        }

        if (FAILED(device.As(&m_d3dDevice))) {
            std::cout << "device.AS() failed.\n";
            return false;
        }

        if (FAILED(context.As(&m_d3dContext))) {
            std::cout << "context.As() failed.\n";
            return false;
        }

         if (FAILED(D3D11CreateDeviceAndSwapChain(0, // Default adapter
            driverType,
            0, // No software device
            createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION,
            &sd, &m_swapChain, &m_d3dDevice, &featureLevel,
            &m_d3dContext))) {
            std::cout << "D3D11CreateDeviceAndSwapChain() failed.\n";
            return false;
        }

        // CreateRenderTarget
        ID3D11Texture2D* pBackBuffer;
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer) {
            m_d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
            pBackBuffer->Release();
        }
        else {
            std::cout << "CreateRenderTargetView() failed.\n";
            return false;
        }

        // Set the viewport
        ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
        m_screenViewport.TopLeftX = 0;
        m_screenViewport.TopLeftY = 0;
        m_screenViewport.Width = float(m_screenWidth);
        m_screenViewport.Height = float(m_screenHeight);
        // m_screenViewport.Width = static_cast<float>(m_screenHeight);
        m_screenViewport.MinDepth = 0.0f;
        m_screenViewport.MaxDepth = 1.0f; // Note: important for depth buffering

        m_d3dContext->RSSetViewports(1, &m_screenViewport);

        // Create a rasterizer state
        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
        rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rastDesc.FrontCounterClockwise = false;

        m_d3dDevice->CreateRasterizerState(&rastDesc, &m_rasterizerState);

        // Create depth buffer

        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        depthStencilBufferDesc.Width = m_screenWidth;
        depthStencilBufferDesc.Height = m_screenHeight;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Depth 24 bit, Stencil 8 bit
        if (numQualityLevels > 0) {
            depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
            depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
        }
        else {
            depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
            depthStencilBufferDesc.SampleDesc.Quality = 0;
        }
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.MiscFlags = 0;

        if (FAILED(m_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, 0,
            m_depthStencilBuffer.GetAddressOf()))) {
            std::cout << "CreateTexture2D() failed.\n";
        }
        if (FAILED(
            m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, &m_depthStencilView))) {
            std::cout << "CreateDepthStencilView() failed.\n";
        }

        // Create depth stencil state

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true; // false
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
        if (FAILED(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc,
            m_depthStencilState.GetAddressOf()))) {
            std::cout << "CreateDepthStencilState() failed.\n";
        }

        return true;
	}
	bool D3D11AppBase::InitGUI()
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        if (!ImGui_ImplDX11_Init(m_d3dDevice.Get(), m_d3dContext.Get())) {
            return false;
        }

        if (!ImGui_ImplWin32_Init(m_mainWindow)) {
            return false;
        }

        return true;
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

    void D3D11AppBase::CreateVertexShaderAndInputLayout(const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
        ComPtr<ID3D11VertexShader>& vertexShader, ComPtr<ID3D11InputLayout>& inputLayout) {

        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;

        // 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
        HRESULT hr =
            D3DCompileFromFile(filename.c_str(), 0, 0, "main", "vs_5_0", 0, 0, &shaderBlob, &errorBlob);

        CheckResult(hr, errorBlob.Get());

        m_d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
            &vertexShader);

        m_d3dDevice->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            &inputLayout);
    }

    void D3D11AppBase::CreatePixelShader(const std::wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader) {
        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;

        // 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
        HRESULT hr =
            D3DCompileFromFile(filename.c_str(), 0, 0, "main", "ps_5_0", 0, 0, &shaderBlob, &errorBlob);

        CheckResult(hr, errorBlob.Get());

        m_d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
            &pixelShader);
    }

    void D3D11AppBase::CreateIndexBuffer(const std::vector<uint16_t>& indices,
        ComPtr<ID3D11Buffer>& m_indexBuffer) {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = indices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        m_d3dDevice->CreateBuffer(&bufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf());
    }
}