#pragma once

#define USE_FBX_SDK 0
#define USE_ASSIMP 1

#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#define NOMINMAX
//#ifdef max
//    #undef max
//#endif
//#ifdef min
//    #undef min
//#endif
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

#include "D3D11Geometry.h"



inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        // Set a breakpoint on this line to catch Win32 API errors.
        throw std::exception();
    }
}