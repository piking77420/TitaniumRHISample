#include <Common/CameraExemple.hpp>

#include <span>
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

    void CameraExemple::drawRHIInformation()
    {
        const std::string_view api = TiRHI::toString(getRHI().getApi());
        const std::string_view deviceName = getDevice().getSourceAdapter(getRHI()).getName();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 4));
        ImGui::SetNextWindowBgAlpha(0.85f);
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        constexpr float padding = 10.0f;

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - padding,
                                       viewport->WorkPos.y + viewport->WorkSize.y - padding),
                                ImGuiCond_Always, ImVec2(1.0f, 1.0f));

        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                                           /*ImGuiWindowFlags_NoDocking |*/ ImGuiWindowFlags_NoFocusOnAppearing |
                                           ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("RHI Information", nullptr, flags))
        {
            ImVec4 apiColor(0.7f, 0.7f, 0.7f, 1.0f);

            if (api == "Vulkan")
                apiColor = ImVec4(0.95f, 0.3f, 0.3f, 1.0f);
            else if (api == "DirectX12")
                apiColor = ImVec4(0.3f, 0.75f, 0.4f, 1.0f);
            else if (api == "Metal")
                apiColor = ImVec4(0.4f, 0.65f, 1.0f, 1.0f);

            ImGui::TextColored(apiColor, "%.*s", static_cast<int>(api.size()), api.data());
            ImGui::Separator();
            ImGui::TextUnformatted(deviceName.data(), deviceName.data() + deviceName.size());

            std::span<const TiRHI::Adapter::Features> features = getDevice().getFeaturesEnable();
            for (const auto& feature : features)
            {
                const std::string_view view = TiRHI::Adapter::toString(feature);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.5f, 1.0f));

                ImGui::Bullet();
                ImGui::PopStyleColor();

                ImGui::SameLine();

                ImGui::TextDisabled("%.*s", static_cast<int>(view.size()), view.data());
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    void CameraExemple::onUpdate()
    {
        drawRHIInformation();
        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            m_cameraController.update(getIo(), getWindow(), m_camera);
        }

        m_camera.handleDirty();
    }

} // Ti::Common
