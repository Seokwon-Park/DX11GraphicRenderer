#include "D3D11Camera.h"

namespace graphics {
    XMMATRIX D3D11Camera::GetViewRow() {
        // 렌더링에 사용할 View 행렬을 만들어주는 부분
        // 이번 예제에서는 upDir이 Y로 고정되었다고 가정합니다.
        // 시점 변환은 가상 세계가 통째로 반대로 움직이는 것과 동일
        // m_pitch이 고개를 숙이는 회전이라서 -가 두번 붙어서 생략
        
        //XMVECTOR position = XMVectorSet(-this->m_position.x, -this->m_position.y, -this->m_position.z, 1.0f);

        ViewRow = XMMatrixTranslation(-this->m_position.x, -this->m_position.y, -this->m_position.z) *
            XMMatrixRotationY(-this->m_yaw) *
            XMMatrixRotationX(this->m_pitch);
        // TODO:
        //return XMMatrixTranslation(-this->m_position) *
        //    XMMatrixRotationY(-this->m_yaw) *
        //    XMMatrixRotationX(this->m_pitch);
        return ViewRow;
    }

    XMFLOAT3 D3D11Camera::GetEyePos() { return m_position; }

    void D3D11Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
        // 얼마나 회전할지 계산
        // https://en.wikipedia.org/wiki/Aircraft_principal_axes
        m_yaw = mouseNdcX * DirectX::XM_2PI;     // 좌우 360도
        m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도

        // 이동할 때 기준이 되는 정면/오른쪽 방향 계산

        XMVECTOR rot = XMQuaternionRotationRollPitchYaw(-this->m_pitch,this->m_yaw,0.f);
        //TODO:
        //XMStoreFloat3(&m_viewDir, XMVector3Transform(
        //    XMVectorSet(0.0f, 0.0f, 1.0f, 0.f),
        //    XMMatrixRotationY(this->m_yaw)));
        XMStoreFloat3(&m_viewDir, XMVector3Transform(
            XMVectorSet(0.0f, 0.0f, 1.0f, 0.f),
            XMMatrixRotationQuaternion(rot)));
        
            //*XMMatrixRotationX(this->m_pitch))); // m_yaw만큼 회전); // m_yaw만큼 회전
        XMStoreFloat3(&m_rightDir ,XMVector3Cross(XMLoadFloat3(&m_upDir), XMLoadFloat3(&m_viewDir))); // Cross product 사용
    }

    void D3D11Camera::MoveForward(float dt) {
        // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
        XMStoreFloat3(&m_position , 
            XMVectorAdd(XMLoadFloat3(&m_position),
            XMVectorScale( XMLoadFloat3(&m_viewDir), m_speed * dt)));

    }

    void D3D11Camera::MoveRight(float dt) {
        // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
        //m_position += m_rightDir * m_speed * dt;
        XMStoreFloat3(&m_position,
            XMVectorAdd(XMLoadFloat3(&m_position),
                XMVectorScale(XMLoadFloat3(&m_rightDir), m_speed * dt)));
    }

    void D3D11Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

    XMMATRIX D3D11Camera::GetProjRow() {
        return m_usePerspectiveProjection
            ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY),
                m_aspect, m_nearZ, m_farZ)
            : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f,
                1.0f, m_nearZ, m_farZ);
    }
} // namespace graphics