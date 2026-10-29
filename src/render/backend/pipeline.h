#pragma once

#include "context.h"

namespace backend {
    class Pipeline {
        const Context* pContext = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
        vk::raii::PipelineLayout mLayout = nullptr;
        std::vector<vk::raii::ShaderModule> mShaders;
    public:
        bool initialize(const Context& context, const std::vector<std::string>& files);
    private:
        vk::raii::ShaderModule initializeShader(const std::string& file);
        vk::raii::Pipeline initializeGraphicsPipeline();
    };
}
