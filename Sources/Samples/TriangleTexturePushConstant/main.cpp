#include <array>
#include <print>
#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <format>
#include <string>
#include <string_view>
#include <span>

#include <ImGuiIntegration/ImGuiIntegration.hpp>
#include <imgui.h>

#include <Common/Exemple.hpp>
#include <Assets/Image.hpp>
#include <Assets/Texture.hpp>
#include <Rendering/Texture.hpp>
#include <Io/File.hpp>
#include <Core/Log.hpp>

using namespace std::literals;
using namespace Ti::ShaderCompiler;
namespace fs = std::filesystem;

class TriangleTexturePushConstant : public Ti::Common::Exemple
{
public:
    TriangleTexturePushConstant() = delete;
    virtual ~TriangleTexturePushConstant() = default;

    TriangleTexturePushConstant(std::string name)
        : Ti::Common::Exemple(std::move(name))
        , m_pipeline(getRHI())
    {
        setupPipeline();
        setupImgui();
    }

protected:
    std::array<float, 4> m_color = {};

    TiRHI::GraphicPipeline m_pipeline;

    Ti::ThirdParty::ImguiIntegration m_imguiIntegration;

    void setupPipeline()
    {
        const fs::path resourceDir = fs::path(TITANIUM_RESOURCE_DIR) / "triangleColorPushConstant.hlsl";
        const std::vector<Compiler::ShaderEntryPoint> entryPoint = {
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Vertex,
                                       .entryPoint = L"VSMain"sv,
                                       .outPutModuleName = L"triangleColorPushConstant.vert.spriv"},
            Compiler::ShaderEntryPoint{.stage = Compiler::ShaderStage::Pixel,
                                       .entryPoint = L"PSMain"sv,
                                       .outPutModuleName = L"triangleColorPushConstant.pixel.spriv"}};

        std::vector<CompileModuleData> moduleDatas = compileShader(resourceDir.generic_string().c_str(), entryPoint);

        for (auto& moduleDatas : moduleDatas)
            m_pipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        const TiRHI::PushConstant pushConstant{.flag = TiRHI::ShaderStageFlags::Pixel, .size = sizeof(m_color)};

        if (!m_pipeline.setName("Base Pipeline")
                 .setPushConstant(pushConstant)
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

    void setupImgui()
    {
        if (!m_imguiIntegration.build(getDevice(), getWindow(), getSwapChain()))
        {
            TI_LOG_ERROR(Core, "Failed to Init Imgui");
        }
    }

    void render(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
        if (!cmd.beginRecord())
            return;

        {
            TiRHI::DebugScope _(cmd, "SwapChain Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
            TiRHI::RenderTargets& renderTarget = getSwapChain().getCurrentRenderTargets();

            TiRHI::Rect2D renderArea{};
            renderArea.setExtend(getSwapChain().getExtend());

            std::array<TiRHI::ClearValueColor, 1> clearColor;
            clearColor[0].color = AppClearColor;
            TiRHI::BeginRenderPass renderPass{};
            renderPass.setRenderArea(renderArea).setClearColors(clearColor);

            cmd.transitionResource(getSwapChain().getCurrentSwapChainTexture(), TiRHI::ResourceState::Present);
            if (cmd.beginRenderPass(renderPass, renderTarget))
            {
                TiRHI::Viewport viewPort;
                viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

                cmd.setViewPort(viewPort);

                TiRHI::Rect2D scissors;
                scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
                cmd.setScissors(scissors);
                cmd.bindPipeline(m_pipeline);
                cmd.pushConstant(m_color.data(), sizeof(m_color));
                cmd.draw(3, 1, 0, 0);

                {
                    TiRHI::DebugScope _(cmd, "Imgui Pass"sv, std::array{1.0f, 1.0f, 0.0f, 1.0f});
                    m_imguiIntegration.render(cmd);
                }

                cmd.endRenderPass();
            }
        }
        cmd.endRecord();
    }

    void update() final
    {
        m_imguiIntegration.newFrame();
        static std::string windowName = [&]()
        { return std::string(getName()) + " " + std::string(TiRHI::toString(getRHI().getApi())); }();
        ImGui::Begin(windowName.c_str());
        ImGui::ColorPicker4("Triangle color", m_color.data(),
                            ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
        ImGui::End();
        m_imguiIntegration.update();
    }
};

int main()
{
    TriangleTexturePushConstant triangleTexturedExemple("TriangleTexturePushConstant");
    triangleTexturedExemple.run();
    return 0;
}
