#ifndef TITATNIUM_SAMPLE_CAMERA_EXEMPLE_H
#define TITATNIUM_SAMPLE_CAMERA_EXEMPLE_H

#include <Common/Exemple.hpp>
#include <Common/CameraController.hpp>
#include <Core/Camera.hpp>

namespace Ti::Common
{
    class CameraExemple : public Ti::Common::Exemple
    {
    public:
        CameraExemple() = delete;
        virtual ~CameraExemple() = default;

        CameraExemple(std::string name);

        virtual void onUpdate();

    protected:
        Ti::Core::Camera m_camera;

        Ti::Common::CameraController m_cameraController;
    };

} // namespace Ti::Common

#endif // TITATNIUM_SAMPLE_CAMERA_EXEMPLE_H
