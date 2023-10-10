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
	class Model
	{
	public:
		bool Initialize(ComPtr<ID3D11Device>& device, const std::string filename);
		bool Initialize(ComPtr<ID3D11Device>& device, const std::vector<MeshData>& meshes);
		void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
		void Render(ComPtr<ID3D11DeviceContext>& context);

		// ConstantBufferData(Cpp) -> ConstantBuffer(Shader)
		BasicVertexConstantBuffer m_basicVertexConstantBufferData;
		BasicPixelConstantBuffer m_basicPixelConstantBufferData;

		ComPtr<ID3D11ShaderResourceView> diffuseSRV;
		ComPtr<ID3D11ShaderResourceView> specularSRV;

		bool m_drawNormalsDirtyFlag = false;
		bool m_drawNormals = false;


		std::vector<std::shared_ptr<Mesh>> meshes;

		//Shaders
		//Basic
		ComPtr<ID3D11VertexShader> m_basicVS;
		ComPtr<ID3D11InputLayout> m_basicInputLayout;
		ComPtr<ID3D11PixelShader> m_basicPS;

		ComPtr<ID3D11VertexShader> m_normalVS;
		ComPtr<ID3D11PixelShader> m_normalPS;
		ComPtr<ID3D11GeometryShader> m_normalGS;
	
		ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
		ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

		ComPtr<ID3D11Buffer> m_normalVertexConstantBuffer;

		ComPtr<ID3D11SamplerState> m_samplerState;


	};


}