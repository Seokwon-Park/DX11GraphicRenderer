#pragma once
#include "CubeMap.h"

#include "D3D11Core.h"
#include "D3D11Geometry.h"
#include "D3D11Math.h"
#include "D3D11Utilities.h"
#include "Model.h"

namespace graphics
{
	// 4byte = 32bit(float, default int = int32)
	using namespace DirectX;
		
	class D3D11Renderer : public D3D11Core
	{
	public:
		void BuildFilters();

		D3D11Renderer();

		virtual bool Initialize() override;
		virtual void UpdateGUI() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

	protected:
		Model my_Mesh1;
		Model my_Mesh2;

		std::vector<std::shared_ptr<D3D11PostProcess>> m_postProcesses;


		//Normal
		ComPtr<ID3D11VertexShader> m_normalVertexShader;
		ComPtr<ID3D11PixelShader> m_normalPixelShader;

		//Normal
		NormalVertexConstantBuffer m_normalVertexConstantBufferData;

		// perspective vs orthographic
		bool m_usePerspectiveProjection = true;
		
		// Transforms
		XMFLOAT3 m_modelTranslation = XMFLOAT3();
		XMFLOAT3 m_modelRotation = XMFLOAT3();
		XMFLOAT3 m_modelScaling = XMFLOAT3(.5f,.5f,.5f);

		//XMFLOAT3 m_viewEyePos= XMFLOAT3(0.0f,0.0f,-2.0f);
		//XMFLOAT3 m_viewEyeDir= XMFLOAT3(0.0f, 0.0f, 1.0f);
		//XMFLOAT3 m_viewUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMFLOAT3 m_viewRot = CreateXMFLOAT3(0.f);

		//Frustum
		//Field of View
		float m_projFovAngleY = 70.0f;
		float m_nearZ = 0.01f;
		float m_farZ = 100.0f;
		float m_aspect = D3D11Core::GetAspectRatio();		

		//Light
		int m_lightType = 0;
		Light m_lightFromGUI;
		float m_materialDiffuse = .3f;
		float m_materialSpecular = 1.0f;
		XMFLOAT3 m_fresnelR0 = XMFLOAT3(1.0f, 0.71f, 0.29f); // Gold
		
		//Normal
		std::shared_ptr<Mesh> m_normalLines;
		bool m_drawNormals = false;
		bool m_dirtyFlag = false;

		CubeMap m_cubeMap;
		uint8_t m_pickColor[4] = { 0, };

		int m_dirtyflag = 1; // 처음에 한 번 실행
		int m_down = 16;
		int m_repeat = 5;
		float m_threshold = 0.3f;
		float m_strength = 1.0f;
	};
} // namespace graphics