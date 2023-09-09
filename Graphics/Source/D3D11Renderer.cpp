#include "D3D11Renderer.h"

namespace graphics
{
	void D3D11Renderer::EditTransform(D3D11Camera& camera, XMMATRIX& matrix)
	{
		ImGui::Begin("Transform Mode");
	
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)m_screenWidth, (float)m_screenHeight);
		ImGuizmo::Manipulate(camera.GetViewRow().r->m128_f32, camera.GetProjRow().r->m128_f32, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.r->m128_f32);
		//ImGuizmo::ViewManipulate(camera.GetViewRow().r->m128_f32, 3.0f, ImVec2(200, 100), ImVec2(128, 128), 0x10101010);
		//보류
		ImGuizmo::DecomposeMatrixToComponents(matrix.r->m128_f32, matrixTranslation, matrixRotation, matrixScale);
		//matrixRotation[0] /= 180.f;
		//matrixRotation[1] /= 180.f;
		//matrixRotation[2] /= 180.f;
		ImGui::SameLine();
		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}

		//std::cout << m_modelRotation.y << "  " << matrixRotation[1] << std::endl;

		if(ImGuizmo::IsUsing())
		{
			m_modelTranslation.x = matrixTranslation[0];
			m_modelTranslation.y = matrixTranslation[1];
			m_modelTranslation.z = matrixTranslation[2];

			m_modelRotation.x = matrixRotation[0];
			m_modelRotation.y = matrixRotation[1];
			m_modelRotation.z = matrixRotation[2];

			m_modelScaling.x = matrixScale[0];
			m_modelScaling.y = matrixScale[1];
			m_modelScaling.z = matrixScale[2];
		}
		ImGui::End();
	}

	namespace
	{
		void ToggleButton(const char* str_id, bool* v)
		{
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			float height = ImGui::GetFrameHeight();
			float width = height * 1.55f;
			float radius = height * 0.50f;

			if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
				*v = !*v;
			ImU32 col_bg;
			if (ImGui::IsItemHovered())
				col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
			else
				col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

			draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
			draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
		}

		void DrawSingleFloatControl(const std::string& label, float& value,
			float resetValue = 0.f, float min = -1.f, float max = 1.f,
			float columnWidth = 100.f)
		{
			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			float lineHeight = ImGui::GetFrameHeight();
			ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .1f, .1f, .1f, 1.f });
			if (ImGui::Button("X", buttonSize))
				value = resetValue;
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::DragFloat("##X", &value, 1.f, min, max, "%.2f");
			ImGui::SameLine();

			ImGui::Columns(1);

			ImGui::PopID();
		}

		void DrawXMFLOAT3Control(const std::string& label, XMFLOAT3& values,
			float resetValue = 0.f, float min = -1.f, float max = 1.f, 
			float columnWidth = 100.f )
		{
			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();


			float width = ImGui::GetColumnWidth() - columnWidth;

			float lineHeight = ImGui::GetFrameHeight();
			ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .8f, .1f, .1f, 1.f });
			ImGui::PushItemWidth(width / 3);
			if (ImGui::Button("X", buttonSize))
				values.x = resetValue;
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.f, min, max, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .1f, .8f, .1f, 1.f });
			ImGui::PushItemWidth(width / 3);
			if (ImGui::Button("Y", buttonSize))
				values.y = resetValue;
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.f, min, max, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .1f, .1f, .8f, 1.f });
			ImGui::PushItemWidth(width / 3);
			if (ImGui::Button("Z", buttonSize))
				values.z = resetValue;
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.f, min, max, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::Columns(1);

			ImGui::PopID();
		}

	}// anonymouse namespace 
	void D3D11Renderer::CreateBloom() {

		m_postProcesses.clear();

		std::shared_ptr<D3D11PostProcess> copyFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
				m_screenWidth, m_screenHeight);
		copyFilter->SetShaderResources({ this->m_shaderResourceView });
		m_postProcesses.push_back(copyFilter);

		std::shared_ptr<D3D11PostProcess> blurFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
				m_screenWidth, m_screenHeight);
		blurFilter->SetShaderResources({ this->m_shaderResourceView });
		blurFilter->m_pixelConstData.threshold = 0.f;
		blurFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
		m_postProcesses.push_back(blurFilter);
		for (int down = m_down; down >= 1; down /= 2)
		{
			for (int i = 0; i < m_repeat; i++)
			{
				auto& prevResource = m_postProcesses.back()->m_shaderResourceView;
				m_postProcesses.push_back(std::make_shared<D3D11PostProcess>(
					m_d3dDevice, m_d3dContext, L"Sampling", L"Blur",
					m_screenWidth / down, m_screenHeight / down));
				m_postProcesses.back()->SetShaderResources({ prevResource });
			}
			if (down > 1)
			{
				auto upFilter = std::make_shared<D3D11PostProcess>(
					m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
					m_screenWidth / down * 2, m_screenHeight / down * 2);
				upFilter->SetShaderResources({ m_postProcesses.back()->m_shaderResourceView });
				upFilter->m_pixelConstData.threshold = 0.0f;
				upFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
				m_postProcesses.push_back(upFilter);
			}
		}

		auto combineFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Combine",
				m_screenWidth, m_screenHeight);
		combineFilter->SetShaderResources({ copyFilter->m_shaderResourceView,
										   m_postProcesses.back()->m_shaderResourceView });
		combineFilter->SetRenderTargets({ this->m_renderTargetView });
		combineFilter->m_pixelConstData.strength = m_strength;
		combineFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
		m_postProcesses.push_back(combineFilter);
	}

	D3D11Renderer::D3D11Renderer() : D3D11Core() {}


	bool D3D11Renderer::Initialize()
	{

		// Initialize Essentials
		if (!D3D11Core::Initialize())
			return false;

		// 포인트로 빌보드 만들기
		m_billboardPoints.Initialize(m_d3dDevice, { {-0.5f, 0.3f, 3.0f, 1.0f},
												{-0.25f, 0.3f, 3.0f, 1.0f},
												{0.0f, 0.3f, 3.0f, 1.0f},
												{0.25f, 0.3f, 3.0f, 1.0f},
												{0.5f, 0.3f, 3.0f, 1.0f} });

		if (!m_cubeMap.Initialize(
			m_d3dDevice,
			L"../Graphics/DGarden_diffuseIBL.dds",
			L"../Graphics/DGarden_specularIBL.dds"))
			return false;

		//if (!my_Mesh1.Initialize(m_d3dDevice, "c:/zelda/zeldaPosed001.fbx"))
		//	return false;

		XMFLOAT3 center(0.0f, 0.0f, 3.0f);
		float radius = 1.0f;

		std::vector<MeshData> meshes;
		meshes.push_back(Geometry::MakeSphere(1.f, 20, 20));
		//meshes[0].textureFilename = "d:/earth.jpg";

		std::vector<MeshData> meshes2;
		meshes2.push_back(Geometry::MakeCube(1.f, 1.f, 1.f));
		//meshes2[0].textureFilename = "d:/earth.jpg";

		if (!my_Mesh1.Initialize(m_d3dDevice, meshes2))
			return false;

		my_Mesh1.diffuseSRV = m_cubeMap.diffuseSRV;
		my_Mesh1.specularSRV = m_cubeMap.specularSRV;

		m_mainBoundingSphere = BoundingSphere(center, radius);

		if (!my_Mesh2.Initialize(m_d3dDevice, meshes2))
			return false;

		{
			MeshData sphere = Geometry::MakeSphere(0.05f, 10, 10);
			m_cursorSphere.Initialize(m_d3dDevice, { sphere });
			m_cursorSphere.diffuseSRV = m_cubeMap.diffuseSRV;
			m_cursorSphere.specularSRV = m_cubeMap.specularSRV;
			XMMATRIX modelMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
			XMMATRIX invTransposeRow = modelMat;
			invTransposeRow *= XMMatrixTranslation(0.f, 0.f, 0.f);
			XMMatrixTranspose(XMMatrixInverse(nullptr, invTransposeRow));

			m_cursorSphere.m_basicVertexConstantBufferData.world = XMMatrixTranspose(modelMat);
			m_cursorSphere.m_basicVertexConstantBufferData.invTranspose =
				XMMatrixTranspose(invTransposeRow);
			m_cursorSphere.m_basicPixelConstantBufferData.useTexture = false;
			m_cursorSphere.m_basicPixelConstantBufferData.material.diffuse =
				XMFLOAT3(1.0f, 1.0f, 0.0f);
			m_cursorSphere.m_basicPixelConstantBufferData.material.specular =
				XMFLOAT3(0.0f, 0.f, 0.f);
			m_cursorSphere.m_basicPixelConstantBufferData.indexColor = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
			m_cursorSphere.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

			// 동일한 크기와 위치에 BoundingSphere 만들기

		}
		//// 노멀 벡터 그리기
		//// 문제를 단순화하기 위해 InputLayout은 BasicVertexShader와 같이 사용합시다.
		//m_normalLines = std::make_shared<Mesh>();
		//// Mesh *m_normalLines = new Mesh();

		//std::vector<Vertex> normalVertices;
		//std::vector<uint32_t> normalIndices;
		//size_t offset = 0;
		//for (const auto& meshData : meshes) {
		//	for (size_t i = 0; i < meshData.vertices.size(); i++) {

		//		auto v = meshData.vertices[i];

		//		v.texcoord.x = 0.0f; // 시작점 표시
		//		normalVertices.push_back(v);

		//		v.texcoord.x = 1.0f; // 끝점 표시
		//		normalVertices.push_back(v);

		//		normalIndices.push_back(uint32_t(2 * (i + offset)));
		//		normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		//	}
		//	offset += meshData.vertices.size();
		//}

		//D3D11Utilities::CreateVertexBuffer(m_d3dDevice, normalVertices, m_normalLines->vertexBuffer);
		//m_normalLines->m_indexCount = UINT(normalIndices.size());
		//D3D11Utilities::CreateIndexBuffer(m_d3dDevice, normalIndices, m_normalLines->indexBuffer);
		//D3D11Utilities::CreateConstantBuffer(m_d3dDevice, m_normalVertexConstantBufferData,
		//	m_normalLines->vertexConstantBuffer);

		//D3D11Utilities::CreateVertexShaderAndInputLayout(m_d3dDevice,
		//	L"NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
		//	m_colorInputLayout);
		//D3D11Utilities::CreatePixelShader(m_d3dDevice, L"NormalPixelShader.hlsl", m_normalPixelShader);
		CreateBloom();

		//Initialize Success
		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		//ImGui::Checkbox("Use Rim", reinterpret_cast<bool*>(&m_basicPixe
		//auto basicVertexData{ m_basicVertexConstantBufferData };

		if (m_useFPV)
		{
			if (m_keyPressed[87])
				m_camera.MoveForward(dt);
			if (m_keyPressed[83])
				m_camera.MoveForward(-dt);
			if (m_keyPressed[68])
				m_camera.MoveRight(dt);
			if (m_keyPressed[65])
				m_camera.MoveRight(-dt);
		}

		static XMVECTOR prevVector = XMVectorZero();
		XMFLOAT4 q;
		XMFLOAT3 radians(
			XMConvertToRadians(m_modelRotation.x),
			XMConvertToRadians(m_modelRotation.y),
			XMConvertToRadians(m_modelRotation.z)
			);
		XMStoreFloat4(&q, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians)));

		// 모델의 변환
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationX(radians.x)*
			XMMatrixRotationY(radians.y)*
			XMMatrixRotationZ(radians.z)*
			//XMMatrixRotationQuaternion(XMLoadFloat4(&q)) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		EditTransform(m_camera, my_Mesh1.m_basicVertexConstantBufferData.world);
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.world);

		// 역행렬의 전치 행렬
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose = my_Mesh1.m_basicVertexConstantBufferData.world;
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.invTranspose));

		XMMATRIX viewRow = m_camera.GetViewRow();
		XMMATRIX projRow = m_camera.GetProjRow();
		XMFLOAT3 eyeWorld = m_camera.GetEyePos();
		// 시점 변환

		my_Mesh1.m_basicPixelConstantBufferData.eyeWorld = eyeWorld;
		my_Mesh1.m_basicVertexConstantBufferData.view = XMMatrixTranspose(viewRow);
		my_Mesh1.m_basicVertexConstantBufferData.projection = XMMatrixTranspose(projRow);

		m_billboardPoints.m_constantData.eyeWorld = eyeWorld;
		m_billboardPoints.m_constantData.view = XMMatrixTranspose(viewRow);
		m_billboardPoints.m_constantData.proj = XMMatrixTranspose(projRow);



		if (m_pickColor[0] == 255) {
			my_Mesh1.m_basicPixelConstantBufferData.material.diffuse =
				XMFLOAT3(1.0f, 0.1f, 0.1f);
		}

		// 마우스 클릭했을 때만 업데이트
		if (m_leftButton || m_rightButton) {

			// OnMouseMove()에서 m_cursorNdcX, m_cursorNdcY 저장

			// ViewFrustum에서 가까운 면 위의 커서 위치 (z값 주의)
			XMFLOAT3 cursorNdcNear = XMFLOAT3(m_cursorNdcX, m_cursorNdcY, 0.0f);

			// ViewFrustum에서 먼 면 위의 커서 위치 (z값 주의)
			XMFLOAT3 cursorNdcFar = XMFLOAT3(m_cursorNdcX, m_cursorNdcY, 1.0f);

			// NDC 커서 위치를 월드 좌표계로 역변환 해주는 행렬
			XMMATRIX inverseProjView = XMMatrixInverse(nullptr, viewRow * projRow);

			// ViewFrustum 안에서 PickingRay의 방향 구하기
			XMFLOAT3 cursorWorldNear;
			XMStoreFloat3(&cursorWorldNear, XMVector3TransformCoord(XMLoadFloat3(&cursorNdcNear), inverseProjView));
			XMFLOAT3 cursorWorldFar;
			XMStoreFloat3(&cursorWorldFar, XMVector3TransformCoord(XMLoadFloat3(&cursorNdcFar), inverseProjView));

			XMFLOAT3 dir;
			XMStoreFloat3(&dir, XMLoadFloat3(&cursorWorldFar) - XMLoadFloat3(&cursorWorldNear));
			XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));

			// 광선을 만들고 충돌 감지
			Ray curRay = Ray(cursorWorldNear, dir);
			float dist = 0.0f;
			m_selected = curRay.Intersects(m_mainBoundingSphere, dist);
			std::cout << m_selected << std::endl;

			if (m_selected) {
				XMFLOAT3 pickPoint;

				m_cursorSphere.m_basicPixelConstantBufferData.eyeWorld = eyeWorld;

				// 충돌 지점에 작은 구 그리기
				XMStoreFloat3(&pickPoint, XMLoadFloat3(&cursorWorldNear) + XMVectorScale(XMLoadFloat3(&dir), dist));

				XMMATRIX modelMat =
					XMMatrixTranslation(pickPoint.x, pickPoint.y, pickPoint.z); //TODO:

				XMMATRIX invTransposeRow = modelMat;
				invTransposeRow *= XMMatrixTranslation(0.f, 0.f, 0.f);
				XMMatrixTranspose(XMMatrixInverse(nullptr, invTransposeRow));
				m_cursorSphere.m_basicVertexConstantBufferData.world =
					XMMatrixTranspose(modelMat);
				m_cursorSphere.m_basicVertexConstantBufferData.invTranspose =
					XMMatrixTranspose(invTransposeRow);
				m_cursorSphere.m_basicVertexConstantBufferData.view = XMMatrixTranspose(viewRow);
				m_cursorSphere.m_basicVertexConstantBufferData.projection =
					XMMatrixTranspose(projRow);
				m_cursorSphere.m_basicPixelConstantBufferData.indexColor = XMFLOAT4(1.f, 0.f, 0.f, 0.f);
				m_cursorSphere.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

				//if (m_dragStartFlag || m_rightButton) { // 드래그를 시작하는 경우
				//	m_dragStartFlag = false;

				//	prevVector = XMLoadFloat3(&pickPoint)- XMLoadFloat3(&m_mainBoundingSphere.Center);

				//}
				//else {
				//	XMVECTOR currentVector = XMLoadFloat3(&pickPoint) - XMLoadFloat3(&m_mainBoundingSphere.Center);
				//	// TODO:

				//	// 마우스가 조금이라도 움직였을 경우에만 회전시키기
				//	if (XMVectorGetX(XMVector3Length(currentVector - prevVector)) > 1e-3) {
				//		XMStoreFloat4A(&q, XMQuaternionDot(prevVector, currentVector));
				//		//q = Quaternion::FromToRotation(prevVector, currentVector);
				//		// TODO: Quaternion::FromToRotation() 사용

				//		prevVector = currentVector;
				//	}
				//}
			}
		}


		/*my_Mesh1.m_basicVertexConstantBufferData.view =
			XMMatrixRotationX(m_viewRot.x) *
			XMMatrixRotationY(m_viewRot.y) *
			XMMatrixRotationZ(m_viewRot.z) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&my_Mesh1.m_basicPixelConstantBufferData.eyeWorld, XMXMFLOAT3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, my_Mesh1.m_basicVertexConstantBufferData.view)));

		my_Mesh1.m_basicVertexConstantBufferData.view =
			XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.view);*/

			//// 프로젝션
			//m_aspect = D3D11Core::GetAspectRatio();
			//if (m_usePerspectiveProjection) {
			//	my_Mesh1.m_basicVertexConstantBufferData.projection =
			//		XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
			//}
			//else {
			//	my_Mesh1.m_basicVertexConstantBufferData.projection =
			//		XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
			//}
			//my_Mesh1.m_basicVertexConstantBufferData.projection = XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.projection);

			// Constant를 CPU에서 GPU로 복사
		D3D11Utilities::UpdateBuffer(m_d3dContext, my_Mesh1.m_basicVertexConstantBufferData, my_Mesh1.meshes[0]->vertexConstantBuffer);
		D3D11Utilities::UpdateBuffer(m_d3dContext, my_Mesh2.m_basicVertexConstantBufferData, my_Mesh2.meshes[0]->vertexConstantBuffer);
		D3D11Utilities::UpdateBuffer(m_d3dContext, m_billboardPoints.m_constantData,
			m_billboardPoints.m_constantBuffer);

		// light별 fallofEnd fallofStart값을 변경하지 않기 위해 복사해서 사용
		auto basicPixelData{ my_Mesh1.m_basicPixelConstantBufferData };
		auto basicPixelData2{ my_Mesh2.m_basicPixelConstantBufferData };

		// 여러 개 조명 사용 예시
		for (int i = 0; i < MAX_LIGHTS; i++) {
			// 다른 조명 끄기
			if (i != m_lightType) {
				XMVECTOR strength = XMLoadFloat3(&basicPixelData.lights[i].strength);
				XMStoreFloat3(&basicPixelData.lights[i].strength, XMVectorScale(strength, 0.0f));
			}
			else {
				basicPixelData.lights[i] = m_lightFromGUI;
			}
		}

		D3D11Utilities::UpdateBuffer(m_d3dContext, basicPixelData,
			my_Mesh1.meshes[0]->pixelConstantBuffer);
		D3D11Utilities::UpdateBuffer(m_d3dContext, basicPixelData,
			my_Mesh2.meshes[0]->pixelConstantBuffer);
		//// 노멀 벡터 그리기
		//if (m_drawNormals && m_dirtyFlag) {

		//	D3D11Utilities::UpdateBuffer(m_d3dContext, m_normalVertexConstantBufferData,
		//		m_normalLines->vertexConstantBuffer);

		//	m_dirtyFlag = false;

		//}

		// 큐브매핑을 위한 ConstantBuffers
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		// Transpose()도 생략 가능
		m_cubeMap.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

		// Mesh의 MVP를 큐브맵의 vertexConstants로 전달
		D3D11Utilities::UpdateBuffer(m_d3dContext, my_Mesh1.m_basicVertexConstantBufferData,
			m_cubeMap.cubeMesh->vertexConstantBuffer);

		my_Mesh1.m_basicPixelConstantBufferData.material.diffuse =
			CreateXMFLOAT3(m_materialDiffuse);
		my_Mesh1.m_basicPixelConstantBufferData.material.specular =
			CreateXMFLOAT3(m_materialSpecular);
		my_Mesh1.m_basicPixelConstantBufferData.material.fresnelR0 =
			m_fresnelR0;

		// post effects
		if (m_dirtyflag) {
			m_postProcesses[1]->m_pixelConstData.threshold = m_threshold;
			m_postProcesses[1]->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
			m_postProcesses.back()->m_pixelConstData.strength = m_strength;
			m_postProcesses.back()->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
			m_dirtyflag = 0;
		}
	}

	void D3D11Renderer::Render()
	{
		//Graphics Pipeline
		// IA -> VS -> (HS -> TS -> DS) -> GS -> RS -> PS -> OM
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// HS: Hull Shader
		// TS: Tessellator 
		// DS: Domain Shader
		// GS: Geometry Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		m_d3dContext->RSSetViewports(1, &m_screenViewport);

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 검은색으로 클리어
		m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_d3dContext->ClearRenderTargetView(m_indexRenderTargetView.Get(), clearColor);

		m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* rtvs[] = {
			m_renderTargetView.Get(),
			m_indexRenderTargetView.Get()
		};

		m_d3dContext->OMSetRenderTargets(2, rtvs, m_depthStencilView.Get());
		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(my_Mesh1.m_colorVertexShader.Get(), 0, 0);

		m_d3dContext->PSSetSamplers(0, 1, my_Mesh1.m_samplerState.GetAddressOf());
		m_d3dContext->PSSetShader(my_Mesh1.m_colorPixelShader.Get(), 0, 0);

		if (m_drawAsWire)
			m_d3dContext->RSSetState(m_wiredRasterizerState.Get());
		else
			m_d3dContext->RSSetState(m_solidRasterizerState.Get());



		//if (m_drawNormals) {
		//	m_d3dContext->VSSetShader(m_normalVertexShader.Get(), 0, 0);

		//	m_d3dContext->VSSetConstantBuffers(1, 1, m_normalLines->vertexConstantBuffer.GetAddressOf());

		//	m_d3dContext->PSSetShader(m_normalPixelShader.Get(), 0, 0);
		//	// m_d3dContext->IASetInputLayout(m_basicInputLayout.Get());
		//	m_d3dContext->IASetVertexBuffers(
		//		0, 1, m_normalLines->vertexBuffer.GetAddressOf(), &stride,
		//		&offset);
		//	m_d3dContext->IASetIndexBuffer(m_normalLines->indexBuffer.Get(),
		//		DXGI_FORMAT_R32_UINT, 0);

		//	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//	m_d3dContext->DrawIndexed(m_normalLines->m_indexCount, 0, 0);
		//}
		m_billboardPoints.Render(m_d3dContext);


		my_Mesh1.Render(m_d3dContext);
		//my_Mesh2.Render(m_d3dContext);
		if (m_leftButton && m_selected)
			m_cursorSphere.Render(m_d3dContext);
		m_cubeMap.Render(m_d3dContext);

		ComPtr<ID3D11Texture2D> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		m_d3dContext->ResolveSubresource(m_tempTexture.Get(), 0, backBuffer.Get(), 0,
			DXGI_FORMAT_R8G8B8A8_UNORM);
		if (m_usePostProcessing)
		{
			for (auto& m_postProcess : m_postProcesses) {
				m_postProcess->Render(m_d3dContext);
			}
		}

		m_d3dContext->ResolveSubresource(m_indexTempTexture.Get(), 0,
			m_indexTexture.Get(), 0,
			DXGI_FORMAT_R8G8B8A8_UNORM);


		//1x1 pixel box
		D3D11_BOX box;

		box.left = std::clamp(m_cursorX, 0, m_screenWidth);
		box.right = std::clamp(m_cursorX + 1, 0, m_screenWidth);
		box.top = std::clamp(m_cursorY, 0, m_screenHeight);
		box.bottom = std::clamp(m_cursorY + 1, 0, m_screenHeight);

		// if 3D Texture
		box.front = 0;
		box.back = 1;

		m_d3dContext->CopySubresourceRegion(m_indexStagingTexture.Get(), 0, 0, 0, 0,
			m_indexTempTexture.Get(), 0, &box);

		D3D11_MAPPED_SUBRESOURCE ms;
		m_d3dContext->Map(m_indexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
			&ms); // D3D11_MAP_READ 주의
		memcpy(m_pickColor, ms.pData, sizeof(uint8_t) * 4);
		m_d3dContext->Unmap(m_indexStagingTexture.Get(), NULL);

	}

	void D3D11Renderer::UpdateGUI()
	{
		CreateDockingWindow();
		
		//Scene Control 파트
		ImGui::Begin("Scene Control");
		//ImGui::Checkbox("Use Rim", reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useRim));
		//ImGui::SliderFloat("Rim Strength",
		//	&m_basicPixelConstantBufferData.rimStrength, 0.0f,
		//	10.0f);
		//ImGui::Checkbox("Use Smoothstep",
		//	reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useSmoothstep));
		//ImGui::SliderFloat3("Rim Color", &m_basicPixelConstantBufferData.rimColor.x,
		//	0.0f, 1.0f);
		//ImGui::SliderFloat("Rim Power", &m_basicPixelConstantBufferData.rimPower,
		//	0.01f, 10.0f);
		// ToggleButton("Toggle Test", &m_usePostProcessing);
		// ImGui가 측정해주는 Framerate 출력
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Checkbox("Use Texture", &my_Mesh1.m_basicPixelConstantBufferData.useTexture);
		ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
		ImGui::Checkbox("drawAsWire", &m_drawAsWire);
		DrawSingleFloatControl("CameraSpeed", m_camera.m_speed, 1.f, 1.f, 8.f);

		ImGui::Checkbox("use PostProcess", &m_usePostProcessing);
		ImGui::SetNextItemOpen(m_usePostProcessing, ImGuiCond_Always);
		if (ImGui::TreeNode("PostProcess"))
		{
			m_dirtyflag +=
				ImGui::SliderFloat("Bloom Threshold", &m_threshold, 0.0f, 1.0f);
			m_dirtyflag +=
				ImGui::SliderFloat("Bloom Strength", &m_strength, 0.0f, 3.0f);

			ImGui::TreePop();
		}
		ImGui::End();

		ImGui::Begin("Mesh");
		//Transform 제어
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform")) {
			DrawXMFLOAT3Control("Translation", m_modelTranslation, 0.f,  -10.f, 10.f);
			DrawXMFLOAT3Control("Rotation", m_modelRotation, 0.f,  -180.f, 180.f);
			DrawXMFLOAT3Control("Scale", m_modelScaling, 1.f,  .01f, 1.f);

			ImGui::TreePop();
		}
		ImGui::End();

		//조명 파트
		ImGui::Begin("Lights");
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Lights")) {


			if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
				m_lightType = 0;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
				m_lightType = 1;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
				m_lightType = 2;
			}

			ImGui::ColorEdit3("Material Fresnel", &m_fresnelR0.x);

			ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 1.0f);
			ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 1.0f);
			ImGui::SliderFloat("Material Shininess",
				&my_Mesh1.m_basicPixelConstantBufferData.material.shininess, 0.01f,
				20.0f);

			ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f,
				5.0f);

			ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f,
				5.0f);

			ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f,
				10.0f);

			ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f,
				512.0f);
			ImGui::TreePop();
		}

		ImGui::End();
	}
}
