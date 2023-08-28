#pragma once

#include "CommonHeaders.h"
#include "Mesh.h"
#include "BasicConstants.h"
#include "D3D11Core.h"
#include "D3D11Geometry.h"
#include "D3D11Utilities.h"

namespace graphics
{
	using Microsoft::WRL::ComPtr;

	class CubeMap
	{
	public:
		bool Initialize(
			ComPtr<ID3D11Device> device,
			const wchar_t* diffuseDDS,
			const wchar_t* specularDDS
			);

		void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
			ComPtr<ID3D11DeviceContext>& context);

		void Render(ComPtr<ID3D11DeviceContext>& context);

		std::shared_ptr<Mesh> cubeMesh;

		ComPtr<ID3D11ShaderResourceView> diffuseSRV;
		ComPtr<ID3D11ShaderResourceView> specularSRV;

		BasicVertexConstantBuffer vertexConstantData;
		ComPtr<ID3D11SamplerState> samplerState;

		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;
		ComPtr<ID3D11InputLayout> inputLayout;

	};
}
