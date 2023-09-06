#pragma once

#include "D3D11CommonHeaders.h"
#include "D3D11Math.h"

namespace graphics {

    class D3D11Camera {
    public:
        XMMATRIX GetViewRow();
        XMMATRIX GetProjRow();
        XMFLOAT3 GetEyePos();

        void UpdateMouse(float mouseNdcX, float mouseNdcY);
        void MoveForward(float dt);
        void MoveRight(float dt);
        void SetAspectRatio(float aspect);

    private:
        // 1��Ī ������ FPS ������ ���ø��ø� �˴ϴ�.
        // ���� ���迡 1��Ī ���� ���� ĳ���Ͱ� ���ִ� ��Ȳ�Դϴ�.
        // �� ĳ������ ���� ��ġ�� ī�޶� �ֽ��ϴ�.
        // �� ĳ������ ���� ������ ī�޶� ���� �����Դϴ�.

        // m_position : ���� ��ǥ�迡�� ī�޶��� ��ġ
        // m_viewDir : ī�޶� ���� ����, �ɾ�� ����
        // m_upDir : ���� ����, �߷��� �ݴ������ �⺻
        // m_rightDir : ������ ����, eyeDir�� upDir�κ��� ���

        XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.4f, 0.0f); // 0.15f�� ������ ����
        XMFLOAT3 m_viewDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
        XMFLOAT3 m_upDir = XMFLOAT3(0.0f, 1.0f, 0.0f); // calculate 
        XMFLOAT3 m_rightDir = XMFLOAT3(1.0f, 0.0f, 0.0f);

        // roll, pitch, yaw
        // https://en.wikipedia.org/wiki/Aircraft_principal_axes
        float m_roll = 0.0f;
        float m_pitch = 0.0f;
        float m_yaw = 0.0f;

        float m_speed = 1.0f; // �����̴� �ӵ�

        // Proj Option
        float m_projFovAngleY = 70.0f;
        float m_nearZ = 0.01f;
        float m_farZ = 100.0f;
        float m_aspect = 16.0f / 9.0f;
        bool m_usePerspectiveProjection = true;
    };
} // namespace graphics