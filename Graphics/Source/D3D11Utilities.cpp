#include "D3D11Utilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	} // anonymous namespace

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
		const std::wstring& filename,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>& vertexShader,
		ComPtr<ID3D11InputLayout>& inputLayout) 
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
		HRESULT hr =
			D3DCompileFromFile(
				filename.c_str(),
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

	void D3D11Utilities::CreateGeometryShader(
		ComPtr<ID3D11Device>& device, 
		const std::wstring& filename,
		ComPtr<ID3D11GeometryShader>& geometryShader) 
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 쉐이더의 시작점의 이름이 "main"인 함수로 지정
		// D3D_COMPILE_STANDARD_FILE_INCLUDE 추가: 쉐이더에서 include 사용
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
			"gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

		// CheckResult(hr, errorBlob.Get());

		device->CreateGeometryShader(shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(), NULL,
			&geometryShader);
	}

	void D3D11Utilities::CreatePixelShader(
		ComPtr<ID3D11Device>& device, 
		const std::wstring& filename,
		ComPtr<ID3D11PixelShader>& pixelShader) 
	{
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(),
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

	void D3D11Utilities::CreateTexture(
		ComPtr<ID3D11Device> device,
		const std::string filename,
		ComPtr<ID3D11Texture2D>& texture,
		ComPtr<ID3D11ShaderResourceView>& textureResourceView)
	{
		int width, height, channels;

		unsigned char* img =
			stbi_load(filename.c_str(), &width, &height, &channels, 0);

		//assert(channels == 4);

		std::vector<uint8_t> image;

		// 4채널로 만들어서 복사
		image.resize(width * height * 4);
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 3; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}

		// Create texture.
		D3D11_TEXTURE2D_DESC txtDesc = {};
		txtDesc.Width = width;
		txtDesc.Height = height;
		txtDesc.MipLevels = txtDesc.ArraySize = 1;
		txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		txtDesc.SampleDesc.Count = 1;
		txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
		txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = image.data();
		InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
		// InitData.SysMemSlicePitch = 0;

		device->CreateTexture2D(&txtDesc, &InitData, texture.GetAddressOf());
		device->CreateShaderResourceView(texture.Get(), nullptr,
			textureResourceView.GetAddressOf());
	}

	void D3D11Utilities::CreateTextureArray(
		ComPtr<ID3D11Device> device,
		const std::vector<std::string> filenames,
		ComPtr<ID3D11Texture2D>& texture,
		ComPtr<ID3D11ShaderResourceView>& textureResourceView)
	{
		int width, height, channels;

		std::vector<uint8_t> imageArray;
		unsigned char* img = nullptr;

		for(auto filename:filenames)
		img =
			stbi_load(filename.c_str(), &width, &height, &channels, 0);

		//assert(channels == 4);


		// 4채널로 만들어서 복사
		imageArray.resize(width * height * 4);
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 3; c++) {
				imageArray[4 * i + c] = img[i * channels + c];
			}
			imageArray[4 * i + 3] = 255;
		}

		// Create texture.
		D3D11_TEXTURE2D_DESC txtDesc = {};
		txtDesc.Width = width;
		txtDesc.Height = height;
		txtDesc.MipLevels = txtDesc.ArraySize = 1;
		txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		txtDesc.SampleDesc.Count = 1;
		txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
		txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		// Fill in the subresource data.
		std::vector<D3D11_SUBRESOURCE_DATA> initData(filenames.size());
		size_t offset = 0;
		for (auto& i : initData) {
			i.pSysMem = imageArray.data() + offset;
			i.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
			i.SysMemSlicePitch =
				txtDesc.Width * txtDesc.Height * sizeof(uint8_t) * 4;
			offset += i.SysMemSlicePitch;
		}

		device->CreateTexture2D(&txtDesc, initData.data(), texture.GetAddressOf());
		device->CreateShaderResourceView(texture.Get(), nullptr,
			textureResourceView.GetAddressOf());
	}
}