#include <Common/CameraExemple.hpp>

#include <span>
#include <Imgui.h>

namespace Ti::Common
{

    CameraExemple::CameraExemple(std::string name)
        : Ti::Common::Exemple(std::move(name))
        , m_samplerNearestRepeat(getDevice())
        , m_samplerNearestClamp(getDevice())
        , m_sceneViewTexture(getDevice())
        , m_sceneViewDescriptorSet(getDevice())
        , m_drawTextureToScreenTriangle(getDevice())

    {
        m_camera.setNear(0.1f).setFar(1000.0f).setFov(1.39626f);
        m_camera.setAspectRatio(static_cast<float>(getWindow().getWidth()) /
                                static_cast<float>(getWindow().getHeight()));
        m_camera.handleDirty();

        setupSampler();
        setupSceneView();
        setupDrawSceneViewPipeline();
    }

    void CameraExemple::setupSampler()
    {
        bool success = m_samplerNearestRepeat.setName("SamplerNearestRepeat")
                           .setMinFilter(TiRHI::Filter::Nearest)
                           .setMagFilter(TiRHI::Filter::Nearest)
                           .setMipMapFilter(TiRHI::Filter::Nearest)
                           .setAddressModeU(TiRHI::SamplerAddressMode::Repeat)
                           .setAddressModeV(TiRHI::SamplerAddressMode::Repeat)
                           .setAddressModeW(TiRHI::SamplerAddressMode::Repeat)
                           .setMinLod(0.0f)
                           .setAnisotropyEnabled(false)
                           .setDepthCompareEnabled(false)
                           .build();

        success = success && m_samplerNearestClamp.setName("SamplerNearestClamp")
                                 .setMinFilter(TiRHI::Filter::Nearest)
                                 .setMagFilter(TiRHI::Filter::Nearest)
                                 .setMipMapFilter(TiRHI::Filter::Nearest)
                                 .setAddressModeU(TiRHI::SamplerAddressMode::ClampToEdge)
                                 .setAddressModeV(TiRHI::SamplerAddressMode::ClampToEdge)
                                 .setAddressModeW(TiRHI::SamplerAddressMode::ClampToEdge)
                                 .setMinLod(0.0f)
                                 .setAnisotropyEnabled(false)
                                 .setDepthCompareEnabled(false)
                                 .build();
        if (!success)
        {
            TI_LOG_FATAL(Rendering, "Failed to build sampler");
        }
    }

    void CameraExemple::setupSceneView()
    {
        if (!m_sceneViewTexture.setName("SceneViewTexture")
                 .setFlags(TiRHI::TextureFlags::RenderTarget | TiRHI::TextureFlags::ShaderResource)
                 .setType(TiRHI::TextureType::Texture2D)
                 .setWidth(static_cast<uint32_t>(getSwapChain().getWidth()))
                 .setHeight(static_cast<uint32_t>(getSwapChain().getHeight()))
                 .setFormat(getSwapChain().getFormat())
                 .build())
        {
            TI_LOG_FATAL(Rendering, "Failed to build {}", m_sceneViewTexture.getName());
        }

        if (!m_sceneViewDescriptorSet.setName("SceneViewDescriptorSet")
                 .bindSampledTexture(TiRHI::ShaderStageFlags::Pixel, &m_sceneViewTexture, &m_samplerNearestClamp, 0)
                 .build())
        {
            TI_LOG_FATAL(Rendering, "Failed to build {}", m_sceneViewDescriptorSet.getName());
        }
    }

    void CameraExemple::onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        TiRHI::Viewport viewPort;
        viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

        cmd.setViewPort(viewPort);

        TiRHI::Rect2D scissors;
        scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
        cmd.setScissors(scissors);
        cmd.bindPipeline(m_drawTextureToScreenTriangle);
        cmd.bindDescriptorSet(m_sceneViewDescriptorSet, 0);

        cmd.draw(3, 1, 0, 0);
    }

    void CameraExemple::setupDrawSceneViewPipeline()
    {
        m_drawTextureToScreenTriangle = std::move(compilePsoVertexFrag("drawTextureToScreenTriangle.hlsl",
                                                                       L"drawTextureToScreenTriangle.vert.spriv",
                                                                       L"drawTextureToScreenTriangle.pixel.spriv"));

        if (!m_drawTextureToScreenTriangle.setName("DrawTextureToScreenTriangle Pipeline")
                 .setDescriptorSetLayoutEntries(std::array{
                     TiRHI::GraphicPipeline::DescriptorSetLayoutEntry{m_sceneViewDescriptorSet.getBindingsLayout(), 0}})
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build())
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline {}", m_drawTextureToScreenTriangle.getName());
        }
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

    void CameraExemple::onResizeWindow()
    {
        const float aspect = static_cast<float>(getWindow().getWidth()) / static_cast<float>(getWindow().getHeight());
        m_camera.setAspectRation(aspect);
    }

} // Ti::Common
