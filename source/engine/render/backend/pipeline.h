#pragma once

#include "context.h"
#include "utils.h"
#include "swapchain.h"

namespace backend {
    class Pipeline {
        static inline const Context* sContext;
        static inline const std::string cShadersDirectory = GDRE_SHADERS_DIRECTORY;

        // Objects
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
        vk::PipelineBindPoint mBindPoint;
    public:
        static inline const vk::PipelineRenderingCreateInfo cRenderingInfo{
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &Swapchain::cImageFormat,
            .depthAttachmentFormat = vk::Format::eD32Sfloat
        };

        bool initialize(const Context& context, PipelineType type, const std::vector<std::string>& shaderFiles, const vk::raii::DescriptorSetLayout& descriptorLayout, const std::vector<PushConstants>& pushConstants);

        // Getters
        const vk::raii::PipelineLayout& getLayout() const { return mLayout; }
        const vk::raii::Pipeline& getHandle() const { return mHandle; }
        vk::PipelineBindPoint getBindPoint() const { return mBindPoint; }
    private:
        // Initializers
        static std::vector<vk::raii::ShaderModule> initializeShaders(const std::vector<std::string>& files);
        static vk::raii::PipelineLayout initializeLayout(const vk::raii::DescriptorSetLayout& descriptorLayout, const std::vector<vk::PushConstantRange>& pushConstants);
        static vk::raii::Pipeline initializeGraphicsPipeline(const std::vector<vk::raii::ShaderModule>& shaders, const vk::raii::PipelineLayout& layout);
        static vk::raii::Pipeline initializeComputePipeline();
    };
}
