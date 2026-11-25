#include <Common/CameraController.hpp>
#include <Core/Camera.hpp>
#include <Io/Io.hpp>
#include <Plateform/Window.hpp>
#include <Core/Math/EasingFunction.hpp>
#include <numbers>

namespace Ti::Common
{
    CameraController::CameraController() = default;

    CameraController::~CameraController() = default;

    void CameraController::update(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera)
    {
        if (window.getMouse().inputState.isPressed(Plateform::Mouse::Key::Right))
        {
            setCameraMode(CameraMode::FreeCamera, window);
        }

        if (window.getMouse().inputState.isReleased(Plateform::Mouse::Key::Right))
        {
            setCameraMode(CameraMode::Default, window);
        }

        switch (m_mode)
        {
        case CameraMode::Default:
            updateDefaultCamera(io, window, camera);
            break;
        case CameraMode::FreeCamera:
            updateFreeCamera(io, window, camera);
            break;
        default:
            break;
        }
    }

    CameraController& CameraController::setCameraMode(CameraMode newCameraMode, Ti::Plateform::Window& window)
    {
        if (m_mode != newCameraMode)
        {
            m_mode = newCameraMode;
            switch (m_mode)
            {
            case CameraMode::Default:
                window.setCursorState(Plateform::CursorState::Visible);
                break;
            case CameraMode::FreeCamera:
                break;
            default:
                break;
            }
        }

        return *this;
    }

    void CameraController::updateDefaultCamera(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera)
    {
        // TODO
        const auto& mouse = window.getMouse();
        static constexpr float MinToZoomIn = 0.001f;
        const float wheel = static_cast<float>(mouse.scrollDy);
        if (!MathLib::fuzzyZero(wheel))
        {
            const MathLib::Vector3f fwd = (camera.getAt() - camera.getEye());
            const double lenght = fwd.lengthSquare();
            if (lenght >= (MinToZoomIn * MinToZoomIn))
            {
                const MathLib::Vector3f dtPos = (fwd / lenght) * wheel * DefaultCameraField::ScroolWheelSpeed;
                camera.setEye(camera.getEye() + dtPos);
                camera.setAt(camera.getAt() + dtPos);
            }
        }
    }

    void CameraController::updateFreeCamera(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera)
    {
        const auto& mouse = window.getMouse();

        const bool isRotating = mouse.inputState.isDown(Plateform::Mouse::Key::Right);
        const Plateform::CursorState targetState =
            isRotating ? Plateform::CursorState::Disable : Plateform::CursorState::Visible;
        if (window.getCursorState() != targetState)
        {
            window.setCursorState(targetState);
        }

        const float wheel = static_cast<float>(mouse.scrollDy);
        if (!MathLib::fuzzyZero(wheel))
        {
            m_freeCameraField.speed *= std::pow(1.2f, wheel);
            m_freeCameraField.speed = std::clamp(m_freeCameraField.speed, 1.0f, 1000.0f);
        }

        if (!mouse.inputState.isDown(Plateform::Mouse::Key::Right))
        {
            return;
        }

        updatePosition(io, window.getKeyBoard(), camera);
        updateRotation(io, window.getMouse(), camera);
    }

    void CameraController::updatePosition(const Io::Io& io, const Ti::Plateform::KeyBoard& keyBoard,
                                          Core::Camera& camera)
    {
        using KeyBoard = Ti::Plateform::KeyBoard;
        using Key = KeyBoard::Key;

        const MathLib::Vector3f fwd = (camera.getAt() - camera.getEye()).getNormalize();
        const MathLib::Vector3f right = MathLib::Vector3f::cross(fwd, camera.getUp()).getNormalize();
        MathLib::Vector3f movement = MathLib::Vector3f::zero();

        if (keyBoard.inputState.isDown(Key::W))
            movement += fwd;

        if (keyBoard.inputState.isDown(Key::S))
            movement -= fwd;

        if (keyBoard.inputState.isDown(Key::D))
            movement += right;

        if (keyBoard.inputState.isDown(Key::A))
            movement -= right;

        if (keyBoard.inputState.isDown(Key::E))
            movement += camera.getUp();

        if (keyBoard.inputState.isDown(Key::Q))
            movement -= camera.getUp();

        const double lenghtSq = movement.lengthSquare();
        const bool isMoving = lenghtSq > (0.0001f * 0.0001f);

        if (isMoving)
            movement = movement.getNormalize();

        const float speed = (keyBoard.inputState.isDown(Key::Shift) && isMoving) ? 120.0f : 40.0f;
        const MathLib::Vector3f targetVelocity = movement * m_freeCameraField.speed;
        const float smoothness = isMoving ? m_acceralation : m_deseleration;
        const float alpha =
            Ti::Core::Math::Easing::exponentialEaseOut(smoothness, static_cast<float>(io.getDeltatime()));

        m_velocity += (targetVelocity - m_velocity) * alpha;
        const MathLib::Vector3f dt = m_velocity * io.getDeltatime();
        const MathLib::Vector3f eye = camera.getEye() + dt;
        const MathLib::Vector3f at = camera.getAt() + dt;

        camera.setEye(eye).setAt(at);
    }

    void CameraController::updateRotation(const Io::Io& io, const Plateform::Mouse& mouse, Core::Camera& camera)
    {
        m_freeCameraField.targetYaw +=
            static_cast<float>(mouse.mouseMovementDx) * m_freeCameraField.rotationSensitivity;
        m_freeCameraField.targetPitch +=
            static_cast<float>(mouse.mouseMovementDy) * m_freeCameraField.rotationSensitivity;
        m_freeCameraField.targetPitch = std::clamp(m_freeCameraField.targetPitch, -89.0f, 89.0f);
        const float alpha = Ti::Core::Math::Easing::exponentialEaseOut(m_freeCameraField.rotationSmoothness,
                                                                       static_cast<float>(io.getDeltatime()));
        m_yaw += (m_freeCameraField.targetYaw - m_yaw) * alpha;
        m_pitch += (m_freeCameraField.targetPitch - m_pitch) * alpha;

        constexpr float DegToRad = std::numbers::pi_v<float> / 180.0f;
        const float yaw = DegToRad * m_yaw;
        const float pitch = DegToRad * m_pitch;
        const MathLib::Vector3f fwd =
            MathLib::Vector3f{-std::sin(yaw) * std::cos(pitch), std::sin(pitch), std::cos(yaw) * std::cos(pitch)}
                .normalize();

        camera.setAt(camera.getEye() + fwd);
    }

} // namespace Ti::Common
