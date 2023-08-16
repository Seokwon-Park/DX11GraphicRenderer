#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#include <windows.h>
#include <wrl.h> 

#include <fbxsdk.h>

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

#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\zlib-md.lib")

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                    \
    {                                                                       \
        HRESULT hr__ = (x);                                                 \
        std::wstring wfn = AnsiToWString(__FILE__);                         \
        if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }   \
    }
#endif