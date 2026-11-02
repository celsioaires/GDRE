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
        const backend::Context& context = mBackend->getContext();

        IMGUI_CHECKVERSION();

        if (ImGui::CreateContext() == nullptr) {
            spdlog::error("Failed to initialize ImGui");
            return false;
        }

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
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    void Gui::drawPanels() {
        const ImGuiIO& io = ImGui::GetIO();

        // Window sizes
        const float windowWidth = io.DisplaySize.x;
        const float windowHeight = io.DisplaySize.y;
        const float sidebarWidth = windowWidth * 0.25f;
        const float hierarchyHeight = windowHeight * 0.5f;
        const float inspectorHeight = windowHeight - hierarchyHeight;

        // Window flags
        const ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse;

        // New frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        // Next window
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, hierarchyHeight));

        // Hierarchy
        ImGui::Begin("Hierarchy", nullptr, windowFlags);
        ImGui::End();

        // Next window
        ImGui::SetNextWindowPos(ImVec2(0.0f, hierarchyHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, inspectorHeight));

        // Inspector
        ImGui::Begin("Inspector", nullptr, windowFlags);
        ImGui::End();

        ImGui::Render();
    }

    void Gui::renderPanels() {
        const backend::Swapchain& swapchain = mBackend->getSwapchain();
        const backend::Command& command = mBackend->getCommand();
        const vk::raii::CommandBuffer& commandBuffer = command.getBuffer();

        command.beginRendering(swapchain.getExtent(), vk::AttachmentLoadOp::eLoad, swapchain.getCurrentView());

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);

        commandBuffer.endRendering();
    }
}
