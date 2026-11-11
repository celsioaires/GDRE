#pragma once

#include "context.h"
#include "utils.h"
#include "swapchain.h"

namespace backend {
    class Pipeline {
        static inline const std::string cShadersDirectory = GDRE_SHADERS_DIRECTORY;

        // Objects
        const Context* mContext;
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
        bool initializeShaders(const std::vector<std::string>& files);
        bool initializeLayout(const vk::raii::DescriptorSetLayout& descriptorLayout, const std::vector<vk::PushConstantRange>& pushConstants);
        bool initializeGraphicsPipeline();
        bool initializeComputePipeline();
    };
}
