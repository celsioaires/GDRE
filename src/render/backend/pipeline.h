#pragma once

#include "context.h"

namespace backend {
    enum class PipelineType {
        GRAPHICS,
        COMPUTE,
    };

    class Pipeline {
        const Context* pContext = nullptr;
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
    public:
        bool initialize(const Context& context, PipelineType type, const std::vector<std::string>& files);
    private:
        std::vector<vk::raii::ShaderModule> initializeShaders(const std::vector<std::string>& files);
        vk::raii::PipelineLayout initializeLayout();
        vk::raii::Pipeline initializeGraphicsPipeline();
        vk::raii::Pipeline initializeComputePipeline();
    };
}
