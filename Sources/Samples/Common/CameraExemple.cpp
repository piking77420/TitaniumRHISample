#include <Common/CameraExemple.hpp>
#include <Imgui.h>

namespace Ti::Common
{

    CameraExemple::CameraExemple(std::string name)
        : Ti::Common::Exemple(std::move(name))
    {
        m_camera.setNear(0.1f).setFar(1000.0f).setFov(1.39626f);
        m_camera.setAspectRatio(static_cast<float>(getWindow().getWidth()) /
                                static_cast<float>(getWindow().getHeight()));
        m_camera.handleDirty();
    }

    void CameraExemple::onUpdate()
    {
        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            m_cameraController.update(getIo(), getWindow(), m_camera);
        }

        m_camera.handleDirty();
    }

} // Ti::Common
