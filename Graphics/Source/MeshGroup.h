#pragma once
#include "CommonHeaders.h"
#include "Mesh.h"
#include "BasicConstants.h"

#include "D3D11CommonHeaders.h"
#include "D3D11Geometry.h"
#include "D3D11Utilities.h"
#include "D3D11Core.h"

namespace graphics
{
	class MeshGroup
	{
	public:
		bool Intialize(ComPtr<ID3D11Device>& device, const std::string filename);
		bool Intialize(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes);

		std::vector<std::shared_ptr<Mesh>> meshes;

		//Shaders
		//Basic
		ComPtr<ID3D11VertexShader> m_colorVertexShader;
		ComPtr<ID3D11InputLayout> m_colorInputLayout;
		ComPtr<ID3D11PixelShader> m_colorPixelShader;


		// ConstantBufferData(Cpp) -> ConstantBuffer(Shader)
		BasicVertexConstantBuffer m_basicVertexConstantBufferData;
		BasicPixelConstantBuffer m_basicPixelConstantBufferData;

		ComPtr<ID3D11SamplerState> m_samplerState;
	};


}