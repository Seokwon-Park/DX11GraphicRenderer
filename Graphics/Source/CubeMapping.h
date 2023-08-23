#pragma once

#include "CommonHeaders.h"

namespace graphics
{
	using Microsoft::WRL::ComPtr;

	struct CubeMapping
	{
		std::shared_ptr<Mesh> cubeMesh;

		ComPtr<ID3D11ShaderResourceView> cubemapRV;

		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;
		ComPtr<ID3D11InputLayout> inputLayout;

	};


}
