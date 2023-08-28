#pragma once

#define USE_FBX_SDK 0
#define USE_ASSIMP 1

#include <iostream>
#include <memory>
#include <vector>

#include <windows.h>

//Imgui.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        // Set a breakpoint on this line to catch Win32 API errors.
        throw std::exception();
    }
}