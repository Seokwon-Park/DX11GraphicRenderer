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
        // 1인칭 시점은 FPS 게임을 떠올리시면 됩니다.
        // 가상 세계에 1인칭 시점 게임 캐릭터가 서있는 상황입니다.
        // 그 캐릭터의 눈의 위치에 카메라가 있습니다.
        // 그 캐릭터의 정면 방향이 카메라가 보는 방향입니다.

        // m_position : 월드 좌표계에서 카메라의 위치
        // m_viewDir : 카메라가 보는 방향, 걸어가는 방향
        // m_upDir : 위쪽 방향, 중력의 반대방향이 기본
        // m_rightDir : 오른쪽 방향, eyeDir과 upDir로부터 계산

        XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.4f, 0.0f); // 0.15f는 눈높이 정도
        XMFLOAT3 m_viewDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
        XMFLOAT3 m_upDir = XMFLOAT3(0.0f, 1.0f, 0.0f); // calculate 
        XMFLOAT3 m_rightDir = XMFLOAT3(1.0f, 0.0f, 0.0f);

        // roll, pitch, yaw
        // https://en.wikipedia.org/wiki/Aircraft_principal_axes
        float m_roll = 0.0f;
        float m_pitch = 0.0f;
        float m_yaw = 0.0f;

        float m_speed = 1.0f; // 움직이는 속도

        // Proj Option
        float m_projFovAngleY = 70.0f;
        float m_nearZ = 0.01f;
        float m_farZ = 100.0f;
        float m_aspect = 16.0f / 9.0f;
        bool m_usePerspectiveProjection = true;
    };
} // namespace graphics