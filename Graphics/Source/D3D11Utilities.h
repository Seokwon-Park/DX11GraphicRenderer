#pragma once
#include "D3D11CommonHeaders.h"

#include <wrl.h> 

#include "CommonHeaders.h"

namespace graphics
{
	using namespace Microsoft::WRL;

		class D3D11Utilities
	{
	public:
		// Buffer 관련
		// Template는 헤더 파일에 정의해야 함
		template <typename T_VERTEX>
		static void CreateVertexBuffer(ComPtr<ID3D11Device>& device,
			const std::vector<T_VERTEX>& vertices,
			ComPtr<ID3D11Buffer>& vertexBuffer) {

			// D3D11_USAGE enumeration (d3d11.h)
			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
			bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
			bufferDesc.StructureByteStride = sizeof(T_VERTEX);

			D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 }; // MS 예제에서 초기화하는 방식
			vertexBufferData.pSysMem = vertices.data();
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr =
				device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
			if (FAILED(hr)) {
				std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
			};
		}

		template <typename T_CONSTANT>
		static void CreateConstantBuffer(
			ComPtr<ID3D11Device>& device,
			const T_CONSTANT& constantBufferData,
			ComPtr<ID3D11Buffer>& constantBuffer)
		{
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth = sizeof(constantBufferData);
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;

			// Fill in the subresource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = &constantBufferData;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			auto hr = device->CreateBuffer(&cbDesc, &InitData, constantBuffer.GetAddressOf());

			if (FAILED(hr)) {
				std::cout << "D3D11Utilities::CreateConstantBuffer() CreateBuffer failed()."
					<< std::endl;
			}
		}

		template <typename T_DATA>
		static void UpdateBuffer(
			ComPtr<ID3D11DeviceContext>& context,
			const T_DATA& bufferData,
			ComPtr<ID3D11Buffer>& buffer) {

			if (!buffer) {
				std::cout << "D3D11Utilities::UpdateBuffer() buffer was not initialized."
					<< std::endl;
			}

			D3D11_MAPPED_SUBRESOURCE ms;
			context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, &bufferData, sizeof(bufferData));
			context->Unmap(buffer.Get(), NULL);
		}
	
		static void CreateIndexBuffer(
			ComPtr<ID3D11Device>& device,
			const std::vector<uint32_t>& indices,
			ComPtr<ID3D11Buffer>& m_indexBuffer);
		// Buffer 관련


		//Create Shaders - Pipeline 순서
		static void CreateVertexShaderAndInputLayout(
			ComPtr<ID3D11Device>& device,
			const std::wstring& filename,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			ComPtr<ID3D11VertexShader>& vertexShader,
			ComPtr<ID3D11InputLayout>& inputLayout);

		static void	CreateGeometryShader(
			ComPtr<ID3D11Device>& device, const std::wstring& filename,
				ComPtr<ID3D11GeometryShader>& m_geometryShader);

		static void CreatePixelShader(
			ComPtr<ID3D11Device>& device,
			const std::wstring& filename,
			ComPtr<ID3D11PixelShader>& pixelShader);

		static void CreateTexture(
			ComPtr<ID3D11Device> device,
			const std::string filename,
			ComPtr<ID3D11Texture2D>& texture,
			ComPtr<ID3D11ShaderResourceView>& textureResourceView);

		static void CreateTextureArray(
			ComPtr<ID3D11Device> device,
			const std::vector<std::string> filenames,
			ComPtr<ID3D11Texture2D>& texture,
			ComPtr<ID3D11ShaderResourceView>& textureResourceView);
	};
}