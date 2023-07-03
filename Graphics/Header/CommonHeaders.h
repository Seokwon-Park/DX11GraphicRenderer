#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#include <windows.h>
#include <wrl.h> 


//Imgui.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

//DX11
//#include <dxgi.h> // IDXGIFactory
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Geometry.h"

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                    \
    {                                                                       \
        HRESULT hr__ = (x);                                                 \
        std::wstring wfn = AnsiToWString(__FILE__);                         \
        if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }   \
    }
#endif