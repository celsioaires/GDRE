#include "gui.h"

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#define IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
#include <backends/imgui_impl_vulkan.h>

namespace gui {
    bool Gui::initialize(const render::Renderer& renderer) {
        mBackend = &renderer.getBackend();

        // Context
        IMGUI_CHECKVERSION();

        if (ImGui::CreateContext() == nullptr) {
            spdlog::error("Failed to initialize ImGui");
            return false;
        }

        const backend::Context& context = mBackend->getContext();

        // GLFW backend
        if (!ImGui_ImplGlfw_InitForVulkan(context.getWindow(), true)) {
            spdlog::error("Failed to initialize ImGui GLFW backend");
            return false;
        }

        // Pipeline info
        ImGui_ImplVulkan_PipelineInfo pipelineInfo{
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .PipelineRenderingCreateInfo = backend::Pipeline::cRenderingInfo
        };

        // Vulkan info
        ImGui_ImplVulkan_InitInfo vulkanInfo{
            .ApiVersion = backend::Context::cApiVersion,
            .Instance = *context.getInstance(),
            .PhysicalDevice = *context.getPhysicalDevice(),
            .Device = *context.getDevice(),
            .Queue = *context.getGraphicsQueue(),
            .DescriptorPoolSize = 8,
            .MinImageCount = 3,
            .ImageCount = 3,
            .PipelineInfoMain = pipelineInfo,
            .UseDynamicRendering = true
        };

        // Vulkan backend
        if (!ImGui_ImplVulkan_Init(&vulkanInfo)) {
            spdlog::error("Failed to initialize ImGui Vulkan backend");
            return false;
        }

        return true;
    }

    void Gui::cleanup() {
        // Backends
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    void Gui::prepareWindows() const {
        const ImGuiIO& io = ImGui::GetIO();

        // Sizes
        const float width = io.DisplaySize.x;
        const float height = io.DisplaySize.y;
        const float sidebarWidth = width * 0.25f;
        const float hierarchyHeight = height * 0.5f;
        const float inspectorHeight = height - hierarchyHeight;

        // Flags
        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse;

        // New frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        // Hierarchy
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, hierarchyHeight));

        mHierarchy.draw(flags);

        // Inspector
        ImGui::SetNextWindowPos(ImVec2(0.0f, hierarchyHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, inspectorHeight));

        mInspector.draw(flags);

        // Render frame
        ImGui::Render();
    }

    void Gui::drawWindows() const {
        const backend::Swapchain& swapchain = mBackend->getSwapchain();
        const backend::Command& command = mBackend->getCommand();
        const vk::raii::CommandBuffer& commandBuffer = command.getBuffer();

        // Render commands
        command.beginRendering(swapchain.getExtent(), vk::AttachmentLoadOp::eLoad, swapchain.getCurrentView());

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);

        commandBuffer.endRendering();
    }
}
