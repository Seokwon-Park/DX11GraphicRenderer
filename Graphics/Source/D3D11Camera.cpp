#include "D3D11Camera.h"

namespace graphics {
    XMMATRIX D3D11Camera::GetViewRow() {
        // �������� ����� View ����� ������ִ� �κ�
        // �̹� ���������� upDir�� Y�� �����Ǿ��ٰ� �����մϴ�.
        // ���� ��ȯ�� ���� ���谡 ��°�� �ݴ�� �����̴� �Ͱ� ����
        // m_pitch�� ���� ���̴� ȸ���̶� -�� �ι� �پ ����
        
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
        // �󸶳� ȸ������ ���
        // https://en.wikipedia.org/wiki/Aircraft_principal_axes
        m_yaw = mouseNdcX * DirectX::XM_2PI;     // �¿� 360��
        m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // �� �Ʒ� 90��

        // �̵��� �� ������ �Ǵ� ����/������ ���� ���

        XMVECTOR rot = XMQuaternionRotationRollPitchYaw(-this->m_pitch,this->m_yaw,0.f);
        //TODO:
        //XMStoreFloat3(&m_viewDir, XMVector3Transform(
        //    XMVectorSet(0.0f, 0.0f, 1.0f, 0.f),
        //    XMMatrixRotationY(this->m_yaw)));
        XMStoreFloat3(&m_viewDir, XMVector3Transform(
            XMVectorSet(0.0f, 0.0f, 1.0f, 0.f),
            XMMatrixRotationQuaternion(rot)));
        
            //*XMMatrixRotationX(this->m_pitch))); // m_yaw��ŭ ȸ��); // m_yaw��ŭ ȸ��
        XMStoreFloat3(&m_rightDir ,XMVector3Cross(XMLoadFloat3(&m_upDir), XMLoadFloat3(&m_viewDir))); // Cross product ���
    }

    void D3D11Camera::MoveForward(float dt) {
        // �̵�����_��ġ = ����_��ġ + �̵����� * �ӵ� * �ð�����;
        XMStoreFloat3(&m_position , 
            XMVectorAdd(XMLoadFloat3(&m_position),
            XMVectorScale( XMLoadFloat3(&m_viewDir), m_speed * dt)));

    }

    void D3D11Camera::MoveRight(float dt) {
        // �̵�����_��ġ = ����_��ġ + �̵����� * �ӵ� * �ð�����;
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