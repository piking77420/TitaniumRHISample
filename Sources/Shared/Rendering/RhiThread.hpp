#ifndef TITANIUM_SAMPLE_RENDERING_RHI_THREAD_H
#define TITANIUM_SAMPLE_RENDERING_RHI_THREAD_H

#include <functional>
#include <utility>
#include <vector>

#include <Titanium/TitaniumHeader.hpp>

namespace Ti::Rendering
{
    class RhiThread;

    using RhiCommand = std::move_only_function<void(RhiThread&)>;
    using RhiOnRecordCommand = std::move_only_function<void(TiRHI::Device&, TiRHI::CommandList&)>;

    class RhiThread
    {
    public:
        RhiThread(TiRHI::WindowHandle windowHandle, uint32_t width, uint32_t height);
        ~RhiThread();

        TiRHI::RHI& getRHI()
        {
            return m_rhi;
        }

        const TiRHI::RHI& getRHI() const
        {
            return m_rhi;
        }

        TiRHI::Device& getDevice()
        {
            return m_device;
        }

        TiRHI::SwapChain& getSwapChain()
        {
            return m_swapChain;
        }

        const TiRHI::SwapChain& getSwapChain() const
        {
            return m_swapChain;
        }

        TiRHI::Surface& getSurface()
        {
            return m_surface;
        }

        TiRHI::CommandList& getCommandList()
        {
            return m_cmdList;
        }

        void pushRhiCommand(RhiCommand rhiCommand)
        {
            m_rhiCommands.push_back(std::move(rhiCommand));
        }

        void consumeCommands();

        void pushOnRecordCommand(RhiOnRecordCommand rhiCommand)
        {
            m_rhiCommandOnRecord.push_back(std::move(rhiCommand));
        }

        void consumeOnRecordRhiCommands();

    private:
        static constexpr size_t ImageCount = 3;

        TiRHI::RHI m_rhi;

        TiRHI::Surface m_surface;

        TiRHI::Device m_device;

        TiRHI::SwapChain m_swapChain;

        TiRHI::CommandList m_cmdList;

        // should be double buffered
        std::vector<RhiCommand> m_rhiCommands;

        std::vector<RhiOnRecordCommand> m_rhiCommandOnRecord;
    };

} // namespace Ti::Rendering

#endif // TITANIUM_SAMPLE_RENDERING_RHI_THREAD_H
