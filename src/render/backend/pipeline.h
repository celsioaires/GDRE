#pragma once

#include "context.h"

namespace backend {
    enum class PipelineType {
        GRAPHICS,
        COMPUTE,
    };

    class Pipeline {
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
    public:
        bool initialize(const Context& context, PipelineType type, const std::vector<std::string>& files);

        // Getters
        const vk::raii::Pipeline& getHandle() { return mHandle; }
    private:
        inline static const Context* sContext = nullptr; // const Context& context

        // Initializers
        static std::vector<vk::raii::ShaderModule> initializeShaders(const std::vector<std::string>& files);
        static vk::raii::PipelineLayout initializeLayout();
        static vk::raii::Pipeline initializeGraphicsPipeline(const std::vector<vk::raii::ShaderModule>& shaders, const vk::raii::PipelineLayout& layout);
        static vk::raii::Pipeline initializeComputePipeline();
    };
}
