#ifndef TITATNIUM_SAMPLE_CAMERA_CONTROLLER_H
#define TITATNIUM_SAMPLE_CAMERA_CONTROLLER_H

#include <Vector3.hpp>

namespace Ti::Plateform
{
    struct Mouse;
    struct KeyBoard;
    class Window;
} // Ti::Platform

namespace Ti::Io
{
    class Io;
} // namespace Ti::Io

namespace Ti::Core
{
    class Camera;
} // Ti::Core

namespace Ti::Common
{
    class CameraController
    {
    public:
        CameraController();
        ~CameraController();

        enum class CameraMode : uint8_t
        {
            Default,
            FreeCamera
        };

        void update(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera);

        CameraController& setCameraMode(CameraMode newCameraMode, Ti::Plateform::Window& window);

        CameraMode getCameraMode() const
        {
            return m_mode;
        }

        float getCurrentSpeed() const
        {
            return m_freeCameraField.speed;
        }

    private:
        CameraMode m_mode = CameraMode::Default;

        MathLib::Vector3f m_velocity = MathLib::Vector3<float>{0.0f, 0.0f, 0.0f};

        struct FreeCameraField
        {
            double rotationSensitivity = 0.1;

            float rotationSmoothness = 25.f;

            float targetYaw = 0.0;

            float targetPitch = 0.0;

            float speed = 1.0;

        } m_freeCameraField;

        struct DefaultCameraField
        {
            float targetScrolMovement = 0.0;

            float targetPitch = 0.0;

            static constexpr float ScroolWheelSpeed = 5.0f;
        };

        float m_acceralation = 12.f;

        float m_deseleration = 14.f;

        float m_yaw = 0.0;

        float m_pitch = 0.0;

        void updateDefaultCamera(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera);

        void updateFreeCamera(const Io::Io& io, Ti::Plateform::Window& window, Core::Camera& camera);

        void updatePosition(const Io::Io& io, const Ti::Plateform::KeyBoard& keyBoard, Core::Camera& camera);

        void updateRotation(const Io::Io& io, const Plateform::Mouse& mouse, Core::Camera& camera);
    };

} // namespace Ti::Common

#endif // TITATNIUM_SAMPLE_CAMERA_CONTROLLER_H
