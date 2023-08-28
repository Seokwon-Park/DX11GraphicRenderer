#include "CubeMap.h"

namespace graphics {
	bool CubeMap::Initialize(
		ComPtr<ID3D11Device> device,
		const wchar_t* diffuseDDS,
		const wchar_t* specularDDS
		)
	{

		// texassemble.exe cube -w 2048 -h 2048 -o saintpeters.dds posx.jpg negx.jpg
		// posy.jpg negy.jpg posz.jpg negz.jpg texassemble.exe cube -w 2048 -h 2048
		// -o skybox.dds right.jpg left.jpg top.jpg bottom.jpg front.jpg back.jpg -y
		// https://github.com/Microsoft/DirectXTex/wiki/Texassemble

		// .dds 파일 읽어들여서 초기화
		ComPtr<ID3D11Texture2D> texture;

		D3D11Core::CreateDDSTexture(device, diffuseDDS, diffuseSRV);
		D3D11Core::CreateDDSTexture(device, specularDDS, specularSRV);

		this->cubeMesh = std::make_shared<Mesh>();

		CreateConstantBuffer(device, vertexConstantData,
			cubeMesh->vertexConstantBuffer);

		//MeshData cubeMeshData = Geometry::MakeIcosahedron();
		//MeshData cubeMeshData = Geometry::MakeTetrahedron();
		MeshData cubeMeshData = Geometry::MakeSphere(20.f, 10, 10);
		//MeshData cubeMeshData = Geometry::MakeSphere(1.f, 10, 10);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);

		std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

		CreateVertexBuffer(device, cubeMeshData.vertices,
			cubeMesh->vertexBuffer);
		cubeMesh->m_indexCount = UINT(cubeMeshData.indices.size());
		D3D11Utilities::CreateIndexBuffer(device, cubeMeshData.indices,
			cubeMesh->indexBuffer);

		// 쉐이더 초기화

		// 다른 쉐이더와 동일한 InputLayout 입니다.
		// 실제로는 "POSITION"만 사용합니다.
		std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		D3D11Utilities::CreateVertexShaderAndInputLayout(
			device,
			L"CubeVS.hlsl", basicInputElements,
			vertexShader, inputLayout);

		D3D11Utilities::CreatePixelShader(device, L"CubePS.hlsl",
			pixelShader);

		return true;
	}

	void CubeMap::UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context)
	{
		UpdateBuffer(context, vertexConstantData,
			cubeMesh->vertexConstantBuffer);
	}

	void CubeMap::Render(ComPtr<ID3D11DeviceContext>& context)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		// 큐브매핑
		// 맨 끝에서 렌더링을 진행해도 Depth Buffer가 처리해준다.
		context->IASetInputLayout(inputLayout.Get());
		context->IASetVertexBuffers(
			0, 1, cubeMesh->vertexBuffer.GetAddressOf(), &stride,
			&offset);
		context->IASetIndexBuffer(cubeMesh->indexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->VSSetShader(vertexShader.Get(), 0, 0);
		context->VSSetConstantBuffers(
			0, 1, cubeMesh->vertexConstantBuffer.GetAddressOf());

		//ID3D11ShaderResourceView* views[1] = {
		//	m_cubeMap.specularSRV.Get() };
		ID3D11ShaderResourceView* views[2] = {
			diffuseSRV.Get(),
			specularSRV.Get()
		};

		context->PSSetShaderResources(0, 2, views);

		context->PSSetShader(pixelShader.Get(), 0, 0);
		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		context->DrawIndexed(cubeMesh->m_indexCount, 0, 0);
	}

}
