#pragma once

#include "context.h"
#include "utils.h"
#include "swapchain.h"
#include "descriptor.h"

namespace backend {
    enum PipelineType {
        ePipelineTypeGraphics,
        ePipelineTypeCompute
    };

    class Pipeline {
        // Objects
        const Context* mContext = nullptr;
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
        vk::PipelineBindPoint mBindPoint;
    public:
        bool initialize(const Context& context, PipelineType type, const std::vector<std::string>& shaderFiles, const std::vector<vk::DescriptorSetLayout>& descriptorLayouts, const std::vector<PushConstants>& pushConstants);

        // Getters
        const vk::raii::PipelineLayout& getLayout() const { return mLayout; }
        const vk::raii::Pipeline& getHandle() const { return mHandle; }
        vk::PipelineBindPoint getBindPoint() const { return mBindPoint; }
    private:
        // Initializers
        bool initializeShaders(const std::vector<std::string>& files);
        bool initializeLayout(const std::vector<vk::DescriptorSetLayout>& descriptorLayouts, const std::vector<vk::PushConstantRange>& pushConstants);
        bool initializeGraphicsPipeline();
        bool initializeComputePipeline();

        // Rendering info
        static inline const vk::PipelineRenderingCreateInfo sRenderingInfo{
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &Swapchain::getFormat(),
            .depthAttachmentFormat = vk::Format::eD32Sfloat
        };
    public:
        static vk::PipelineRenderingCreateInfo getRenderingInfo() { return sRenderingInfo; }
    };
}
