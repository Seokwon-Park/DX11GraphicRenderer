#include "GeometryPipeline.h"
#include "D3D11Utilities.h"

namespace graphics
{
	void BillboardPoints::Initialize(ComPtr<ID3D11Device>& device,
		const std::vector<XMFLOAT4>& points) {

		// Sampler 만들기
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

		D3D11Utilities::CreateVertexBuffer(device, points, m_vertexBuffer);

		m_indexCount = uint32_t(points.size());

		m_constantData.width = 2.4f;
		D3D11Utilities::CreateConstantBuffer(device, m_constantData, m_constantBuffer);

		// Geometry shader 초기화하기
		D3D11Utilities::CreateGeometryShader(
			device, L"BillBoardGS.hlsl", m_geometryShader);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, // Vector4
			 D3D11_INPUT_PER_VERTEX_DATA, 0} };

		D3D11Utilities::CreateVertexShaderAndInputLayout(
			device, L"BillBoardVS.hlsl", inputElements,
			m_vertexShader, m_inputLayout);

		D3D11Utilities::CreatePixelShader(device, L"BillBoardPS.hlsl",
			m_pixelShader);

		std::vector<std::string> filenames = {
		"../Assets/Textures/TreeBillboards/1.png",
		"../Assets/Textures/TreeBillboards/2.png",
		"../Assets/Textures/TreeBillboards/3.png",
		"../Assets/Textures/TreeBillboards/4.png",
		"../Assets/Textures/TreeBillboards/5.png" };

		D3D11Utilities::CreateTextureArray(device, filenames, m_texArray,
			m_texArraySRV);
	}

	void BillboardPoints::Render(ComPtr<ID3D11DeviceContext>& context) {

		context->VSSetShader(m_vertexShader.Get(), 0, 0);
		context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		context->PSSetShader(m_pixelShader.Get(), 0, 0);
		context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf());

		context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

		// Geometry Shader
		context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		context->GSSetShader(m_geometryShader.Get(), 0, 0);

		context->IASetInputLayout(m_inputLayout.Get());

		UINT stride = sizeof(XMFLOAT4); // sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
			&offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// 바꿔줄 필요 없이 그대로 POINTLIST 사용
		// context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// POINTLIST는 연결관계가 필요 없기 때문에 DrawIndexed() 대신 Draw() 사용
		context->Draw(m_indexCount, 0);

		// Geometry Shader를 쓰지 않는 다른 물체들을 위해 nullptr로 설정
		context->GSSetShader(nullptr, 0, 0);
	}

} // namespace graphics
