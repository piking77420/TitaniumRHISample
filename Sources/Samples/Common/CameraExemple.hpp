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

        virtual void onResizeWindow();

        virtual void onSwapChainRenderPass(TiRHI::CommandList& cmd, TiRHI::SwapChain& swapChain);

    protected:
        Ti::Core::Camera m_camera;

        Ti::Common::CameraController m_cameraController;

        TiRHI::Sampler m_samplerNearestRepeat;

        TiRHI::Sampler m_samplerNearestClamp;

        TiRHI::Texture m_sceneViewTexture;

        TiRHI::DescriptorSet m_sceneViewDescriptorSet;

        TiRHI::GraphicPipeline m_drawTextureToScreenTriangle;

        void setupSampler();

        void setupSceneView();

        void setupDrawSceneViewPipeline();

        void drawRHIInformation();
    };

} // namespace Ti::Common

#endif // TITATNIUM_SAMPLE_CAMERA_EXEMPLE_H
