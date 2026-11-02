#pragma once

#include "context.h"
#include "constants.h"
#include "swapchain.h"

namespace backend {
    enum PipelineType {
        ePipelineTypeGraphics,
        ePipelineTypeCompute
    };

    class Pipeline {
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
        vk::PipelineBindPoint mBindPoint;
    public:
        // Rendering info
        static inline const vk::PipelineRenderingCreateInfo cRenderingInfo{
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &Swapchain::cImageFormat
        };

        bool initialize(const Context& context, PipelineType type, const std::vector<std::string>& files, Constants constants);

        // Getters
        const vk::raii::PipelineLayout& getLayout() const { return mLayout; }
        const vk::raii::Pipeline& getHandle() const { return mHandle; }
        vk::PipelineBindPoint getBindPoint() const { return mBindPoint; }
    private:
        static inline const Context* sContext = nullptr; // const Context& context
        static inline const std::string cShadersDirectory = GDRE_SHADERS_DIRECTORY;

        // Initializers
        static std::vector<vk::raii::ShaderModule> initializeShaders(const std::vector<std::string>& files);
        static vk::raii::PipelineLayout initializeLayout(vk::PushConstantRange pushConstant);
        static vk::raii::Pipeline initializeGraphicsPipeline(const std::vector<vk::raii::ShaderModule>& shaders, const vk::raii::PipelineLayout& layout);
        static vk::raii::Pipeline initializeComputePipeline();
    };
}
