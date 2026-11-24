#include <ImGuiIntegration/ImGuiIntegration.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#if defined(TITANIUM_VULKAN)
#include <imgui_impl_vulkan.h>
#endif //  defined(TITANIUM_VULKAN)
#if defined(TITANIUM_DIRECT_X12)
#include <backends/imgui_impl_dx12.h>
#include <DirectX12/Private/RhiToDirectX12.hpp>
#endif // defined(TITANIUM_DIRECT_X12)

#include <Plateform/Window.hpp>

#if defined(TITANIUM_VULKAN)
VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
static void check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}
#endif // #if defined(TITANIUM_VULKAN)

namespace Ti::ThirdParty
{
    bool ImguiIntegration::imguiVulkan(const Plateform::Window& window, const TiRHI::SwapChain& swapchain)
    {
#if defined(TITANIUM_VULKAN)
        vk::Device vkDevice = m_device->getNativeDevice();

        // Create Descriptor Pool
        // If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
        {
            VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
                {VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 0;
            for (VkDescriptorPoolSize& pool_size : pool_sizes)
                pool_info.maxSets += pool_size.descriptorCount;
            pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;

            VkResult err = vkCreateDescriptorPool(vkDevice, &pool_info, nullptr, &descriptorPool);
            check_vk_result(err);
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(reinterpret_cast<GLFWwindow*>(window.getGlfwWindow()), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        // init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of
        //  VkApplicationInfo::apiVersion, otherwise will default to header version.
        init_info.Instance = m_device->getRHI().getInstance().getInstance();
        init_info.PhysicalDevice = m_device->getNativePhysicalDevice();
        init_info.Device = vkDevice;
        init_info.QueueFamily = m_device->getNativeGraphicQueueIndex();
        init_info.Queue = m_device->getNativeGraphicQueue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = descriptorPool;
        init_info.MinImageCount = swapchain.getImageCount();
        init_info.ImageCount = swapchain.getImageCount();
        init_info.Allocator = nullptr;
        init_info.PipelineInfoMain.RenderPass = swapchain.getRenderPassDescriptor().getNativeRenderPass();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);
#endif // defined(TITANIUM_VULKAN)

        return true;
    }
    bool ImguiIntegration::imguiD3D12(const Plateform::Window& window, const TiRHI::SwapChain& swapchain)
    {
#if defined(TITANIUM_DIRECT_X12)

        ImGui_ImplDX12_InitInfo info{};

        // Retrieve these from your RHI backend
        info.Device = m_device->getNativeDevice().Get();
        info.CommandQueue = m_device->getNativeGraphicQueue().Get();

        // Number of frames in flight, not swapchain images
        info.NumFramesInFlight = m_device->getRHI().getFrameInFlight();

        // Must match your render target view format
        info.RTVFormat = TiRHI::DirectX12::Private::toDirectX12(swapchain.getFormat());
        info.DSVFormat = DXGI_FORMAT_UNKNOWN;

        // Your existing shader-visible CBV/SRV/UAV heap
        info.SrvDescriptorHeap = TiRHI::Device::Private::getDescriptorAllocatorCbvSrvUav(*m_device).getHeap().Get();

        info.UserData = const_cast<TiRHI::Device*>(m_device);

        info.SrvDescriptorAllocFn =
            [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu)
        {
            auto* device = static_cast<TiRHI::Device*>(info->UserData);

            auto& allocator = TiRHI::Device::Private::getDescriptorAllocatorCbvSrvUav(*device);

            auto handle = allocator.create(*device);

            if (!handle)
            {
                *cpu = {};
                *gpu = {};
                return;
            }

            *cpu = handle->cpuHandle;
            *gpu = handle->gpuHandle;
        };

        info.SrvDescriptorFreeFn =
            [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
        {
            auto* device = static_cast<TiRHI::Device*>(info->UserData);

            TiRHI::DirectX12::Private::DescriptorHandle handle{};
            handle.count = 1;
            handle.cpuHandle = cpu;
            handle.gpuHandle = gpu;

            auto& allocator = TiRHI::Device::Private::getDescriptorAllocatorCbvSrvUav(*device);

            allocator.release(handle);
        };

        return ImGui_ImplDX12_Init(&info);
#endif // defined(TITANIUM_DIRECT_X12)

        return true;
    }

    bool ImguiIntegration::build(const TiRHI::Device& device, Plateform::Window& window,
                                 const TiRHI::SwapChain& swapchain)
    {
        if (!window.getGlfwWindow())
            return false;

        m_device = &device;

        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();
        // style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified, which allows crisp legacy font
        // for non-4K screens.
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;

        // Setup scaling
        style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style
                                         // scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi =
            main_scale; // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically
                        // overrides this for every window depending on the current monitor)

#if defined(TITANIUM_VULKAN)
        return imguiVulkan(window, swapchain);
#endif // defined(TITANIUM_VULKAN)

#if defined(TITANIUM_DIRECT_X12)

        if (!ImGui_ImplGlfw_InitForOther(window.getGlfwWindow(), true))
            return false;

        if (!imguiD3D12(window, swapchain))
        {
            ImGui_ImplGlfw_Shutdown();
            return false;
        }
#endif // defined(TITANIUM_DIRECT_X12)

        return true;
    }

    ImguiIntegration::~ImguiIntegration()
    {
        assert(m_device);
        if (!m_device)
            return;

#if defined(TITANIUM_VULKAN)
        ImGui_ImplVulkan_Shutdown();
        vk::Device vkDevice = m_device->getNativeDevice();
        vkDevice.destroyDescriptorPool(descriptorPool);
#endif // #if defined(TITANIUM_VULKAN)

#if defined(TITANIUM_DIRECT_X12)
        ImGui_ImplDX12_Shutdown();
#endif // #if defined(TITANIUM_DIRECT_X12)

        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiIntegration::newFrame()
    {
#if defined(TITANIUM_VULKAN)
        ImGui_ImplVulkan_NewFrame();
#endif // #if defined(TITANIUM_VULKAN)

#if defined(TITANIUM_DIRECT_X12)
        ImGui_ImplDX12_NewFrame();
#endif // #if defined(TITANIUM_DIRECT_X12)
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiIntegration::update()
    {
        ImGui::Render();
    }

    void ImguiIntegration::render(TiRHI::CommandList& commandList)
    {
        ImDrawData* draw_data = ImGui::GetDrawData();
#if defined(TITANIUM_VULKAN)
        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, commandList.getcurrentFrameCmb());
#endif // #if defined(TITANIUM_VULKAN)
#if defined(TITANIUM_DIRECT_X12)
        ImGui_ImplDX12_RenderDrawData(draw_data, commandList.getCommandListNative());
#endif // #if defined(TITANIUM_DIRECT_X12)
    }
}
