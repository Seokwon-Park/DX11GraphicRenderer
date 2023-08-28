#include "D3D11Utilities.h"

namespace graphics
{
	namespace {
		void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
			if (FAILED(hr)) {
				// 파일이 없을 경우
				if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
					std::cout << "File not found.\n";
				}

				// 에러 메시지가 있으면 출력
				if (errorBlob) {
					std::cout << "Shader compile error\n"
						<< (char*)errorBlob->GetBufferPointer() << '\n';
				}
			}
		}
	}

	void D3D11Utilities::CreateIndexBuffer(
		ComPtr<ID3D11Device>& device, 
		const std::vector<uint32_t>& indices,
		ComPtr<ID3D11Buffer>& m_indexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
		bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		ThrowIfFailed(device->CreateBuffer(&bufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf()));

	}

	void D3D11Utilities::CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>& device,
		const wchar_t* filename,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>& vertexShader,
		ComPtr<ID3D11InputLayout>& inputLayout) {

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
		HRESULT hr =
			D3DCompileFromFile(
				filename,
				0,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				"vs_5_0",
				compileFlags,
				0,
				&shaderBlob,
				&errorBlob);

		CheckResult(hr, errorBlob.Get());

		ThrowIfFailed(device->CreateVertexShader(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			NULL,
			&vertexShader));

		ThrowIfFailed(device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
			shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
			&inputLayout));
	}

	void D3D11Utilities::CreatePixelShader(
		ComPtr<ID3D11Device>& device, 
		const wchar_t* filename,
		ComPtr<ID3D11PixelShader>& pixelShader) {
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
		HRESULT hr = D3DCompileFromFile(
			filename,
			0,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // 쉐이더에서 include를 사용할 수 있도록 설정
			"main",
			"ps_5_0",
			compileFlags,
			0,
			&shaderBlob,
			&errorBlob);

		CheckResult(hr, errorBlob.Get());

		ThrowIfFailed(device->CreatePixelShader(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(), NULL,
			&pixelShader));
	}
}