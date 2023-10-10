#include "Model.h"

namespace graphics
{
	bool Model::Initialize(ComPtr<ID3D11Device>& device, const std::string filename)
	{
		std::vector<MeshData> meshesData = Geometry::ReadModelFromFile(filename);

		if (!Initialize(device, meshesData))
		{
			return false;
		}

		return true;
	}

	bool Model::Initialize(ComPtr<ID3D11Device>& device, const std::vector<MeshData>& meshesData)
	{
		// Texture sampler 만들기
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the Sample State
		device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

		// ConstantBuffer 만들기
		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		m_basicVertexConstantBufferData.view = XMMatrixIdentity();
		m_basicVertexConstantBufferData.projection = XMMatrixIdentity();


		D3D11Utilities::CreateConstantBuffer(device, m_basicVertexConstantBufferData,
			m_vertexConstantBuffer);
		D3D11Utilities::CreateConstantBuffer(device, m_basicPixelConstantBufferData,
			m_pixelConstantBuffer);
		
		m_basicPixelConstantBufferData.indexColor = XMFLOAT4(1.f, 0.0f, 0.0f, 0.0f);


		for (const auto& meshData : meshesData) {
			auto newMesh = std::make_shared<Mesh>();
			D3D11Utilities::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
			newMesh->m_indexCount = UINT(meshData.indices.size());
			newMesh->m_vertexCount = UINT(meshData.vertices.size());
			D3D11Utilities::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

			if (!meshData.textureFilename.empty()) {

				std::cout << meshData.textureFilename << '\n';
				D3D11Utilities::CreateTexture(device, meshData.textureFilename, newMesh->texture,
					newMesh->textureResourceView);
			}

			newMesh->vertexConstantBuffer = m_vertexConstantBuffer;
			newMesh->pixelConstantBuffer = m_pixelConstantBuffer;

			this->meshes.push_back(newMesh);
		}
		
		// 쉐이더 만들기

		// Input-layout objects describe how vertex buffer data is streamed into the
		// IA(Input-Assembler) pipeline stage.
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout

		// Input-Assembler Stage
		// The purpose of the input-assembler stage is to read primitive data
		// (points, lines and/or triangles) from user-filled buffers and assemble
		// the data into primitives that will be used by the other pipeline stages.
		// https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-input-assembler-stage

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			// 4 * 3 은 POSITION = float[3], 4byte*3 다음 데이터부터 시작
		};

		D3D11Utilities::CreateVertexShaderAndInputLayout(device, L"BasicVS.hlsl", inputElements, m_basicVS,
			m_basicInputLayout);
				
		D3D11Utilities::CreatePixelShader(device, L"BasicPS.hlsl", m_basicPS);

		D3D11Utilities::CreateVertexShaderAndInputLayout(device, L"NormalVS.hlsl", inputElements, m_normalVS,
			m_basicInputLayout);
		D3D11Utilities::CreateGeometryShader(device, L"NormalGS.hlsl", m_normalGS);
		D3D11Utilities::CreatePixelShader(device, L"NormalPS.hlsl", m_normalPS);

		return true;
	}
	void Model::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	{
		D3D11Utilities::UpdateBuffer(context, m_basicVertexConstantBufferData,
			m_vertexConstantBuffer);

		D3D11Utilities::UpdateBuffer(context, m_basicPixelConstantBufferData,
			m_pixelConstantBuffer);

		//// 노멀 벡터 그리기
		//if (m_drawNormals && m_drawNormalsDirtyFlag) {
		//	//D3D11Utilities::UpdateBuffer(context, m_normalVertexConstantData,
		//	//	m_normalVertexConstantBuffer);
		//	m_drawNormalsDirtyFlag = false;
		//}
	}

	void Model::Render(ComPtr<ID3D11DeviceContext>& context)
	{

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		// 버텍스/인덱스 버퍼 설정
		for (const auto& mesh : meshes) {

			context->VSSetConstantBuffers(
				0, 1, mesh->vertexConstantBuffer.GetAddressOf());
			context->VSSetShader(m_basicVS.Get(), 0, 0);
			context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
			context->PSSetShader(m_basicPS.Get(), 0, 0);

			ID3D11ShaderResourceView* srv[3] = {
				mesh->textureResourceView.Get(),
				diffuseSRV.Get(),
				specularSRV.Get()
			};

			context->PSSetShaderResources(
				0, 3, srv);

			context->PSSetConstantBuffers(
				0, 1, mesh->pixelConstantBuffer.GetAddressOf());

			context->IASetInputLayout(m_basicInputLayout.Get());
			context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
				&stride, &offset);
			context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(mesh->m_indexCount, 0, 0);
			
			if(m_drawNormals)
			{
				context->VSSetShader(m_normalVS.Get(), 0, 0);

				ID3D11Buffer* pptr[1] = { m_vertexConstantBuffer.Get() };

				context->GSSetConstantBuffers(0, 1, pptr);
				context->GSSetShader(m_normalGS.Get(), 0, 0);
				context->PSSetShader(m_normalPS.Get(), 0, 0);
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				context->Draw(mesh->m_vertexCount, 0);
				context->GSSetShader(nullptr, 0, 0);

			}
		}
	}
}