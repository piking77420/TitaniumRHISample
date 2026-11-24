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
        , m_cubePipeline(getRHI())
    {
        setupPipeline();
    }

protected:
    std::array<float, 4> m_color = {};

    TiRHI::GraphicPipeline m_cubePipeline;

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
            m_cubePipeline.addModule(moduleDatas.stage, std::move(moduleDatas.data), std::move(moduleDatas.entryPoint));

        const TiRHI::PushConstant pushConstant{.flag = TiRHI::ShaderStageFlags::Pixel, .size = sizeof(m_color)};

        if (!m_cubePipeline.setName("Base Pipeline")
                 .setPushConstant(pushConstant)
                 .setRenderPassDescritptor(&getSwapChain().getRenderPassDescriptor())
                 .build(getDevice()))
        {
            TI_LOG_FATAL(Core, "Failed to build pipeline");
        }
    }

    void onRender(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain) final
    {
    }

    void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain)
    {
        TiRHI::Viewport viewPort;
        viewPort.setPosition({0, 0}).setExtend(static_cast<TiRHI::Extend2DF>(getSwapChain().getExtend()));

        cmd.setViewPort(viewPort);

        TiRHI::Rect2D scissors;
        scissors.setOffset({0, 0}).setExtend(getSwapChain().getExtend());
        cmd.setScissors(scissors);
        cmd.bindPipeline(m_cubePipeline);
        cmd.pushConstant(m_color.data(), sizeof(m_color));
        cmd.draw(3, 1, 0, 0);
    }

    void onUpdate() final
    {
        static std::string windowName = [&]()
        { return std::string(getName()) + " " + std::string(TiRHI::toString(getRHI().getApi())); }();
        ImGui::Begin(windowName.c_str());
        ImGui::ColorPicker4("Triangle color", m_color.data(),
                            ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);
        ImGui::End();
    }
};

int main()
{
    TriangleTexturePushConstant triangleTexturedExemple("TriangleTexturePushConstant");
    triangleTexturedExemple.run();
    return 0;
}
