#pragma once

#include "context.h"

namespace backend {
    class Pipeline {
        std::vector<vk::raii::ShaderModule> mShaders;
        vk::raii::PipelineLayout mLayout = nullptr;
        vk::raii::Pipeline mHandle = nullptr;
        vk::PipelineBindPoint mBindPoint;
    public:
        bool initialize(const Context& context, vk::PipelineBindPoint bindPoint, const std::vector<std::string>& files);

        // Getters
        const vk::raii::Pipeline& getHandle() const { return mHandle; }
        vk::PipelineBindPoint getBindPoint() const { return mBindPoint; }
    private:
        static inline const Context* pContext = nullptr; // const Context& context
        static inline const std::string C_SHADERS_DIRECTORY = GDRE_SHADERS_DIRECTORY;

        // Initializers
        static std::vector<vk::raii::ShaderModule> initializeShaders(const std::vector<std::string>& files);
        static vk::raii::PipelineLayout initializeLayout();
        static vk::raii::Pipeline initializeGraphicsPipeline(const std::vector<vk::raii::ShaderModule>& shaders, const vk::raii::PipelineLayout& layout);
        static vk::raii::Pipeline initializeComputePipeline();
    };
}
