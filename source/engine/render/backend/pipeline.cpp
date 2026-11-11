#include "pipeline.h"

#include <fstream>

#include <spdlog/spdlog.h>

namespace backend {
    bool Pipeline::initialize(const Context& context, PipelineType type, const std::vector<std::string>& shaderFiles, const vk::raii::DescriptorSetLayout& descriptorLayout, const std::vector<PushConstants>& pushConstants) {
        mContext = &context;

        // Shaders
        if (!initializeShaders(shaderFiles)) {
            return false;
        }

        // Layout
        std::vector<vk::PushConstantRange> pushConstantRanges;

        for (const PushConstants& pushConstant : pushConstants) {
            pushConstantRanges.push_back({
                .stageFlags = pushConstant.stage,
                .size = sizeof(PushConstantsData)
            });
        }

        if (!initializeLayout(descriptorLayout, pushConstantRanges)) {
            return false;
        }

        // Handle
        switch (type) {
            case ePipelineTypeGraphics: {
                mBindPoint = vk::PipelineBindPoint::eGraphics;
                initializeGraphicsPipeline();
                break;
            }
            case ePipelineTypeCompute: {
                mBindPoint = vk::PipelineBindPoint::eCompute;
                initializeComputePipeline();
                break;
            }
        }

        if (mHandle == nullptr) {
            return false;
        }

        return true;
    }

    bool Pipeline::initializeShaders(const std::vector<std::string> &files) {
        for (const std::string& file : files) {
            // Open stream
            std::ifstream stream(cShadersDirectory + file, std::ios::ate | std::ios::binary);

            if (!stream.is_open()) {
                // Failed
                spdlog::error("Failed to open shader: {}", file);
                return false;
            }

            // Code
            std::vector<char> code(stream.tellg());

            stream.seekg(0, std::ios::beg);
            stream.read(code.data(), static_cast<std::streamsize>(code.size()));

            // Info
            const vk::ShaderModuleCreateInfo createInfo{
                .codeSize = code.size() * sizeof(char),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
            };

            vk::raii::ShaderModule shader = nullptr;

            // Initialize
            try {
                shader = vk::raii::ShaderModule(mContext->getDevice(), createInfo);
                spdlog::info("Shader created successfully: {}", file);
            }
            catch (const vk::SystemError& error) {
                // Failed
                spdlog::error("Failed to create shader: {}\n{}", file, error.what());
                return false;
            }

            mShaders.push_back(std::move(shader));
        }

        return true;
    }

    bool Pipeline::initializeLayout(const vk::raii::DescriptorSetLayout& descriptorLayout, const std::vector<vk::PushConstantRange>& pushConstants) {
        // Info
        const vk::PipelineLayoutCreateInfo info{
            .setLayoutCount = 1,
            .pSetLayouts = &*descriptorLayout,
            .pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size()),
            .pPushConstantRanges = pushConstants.data()
        };

        // Initialize
        try {
            mLayout = vk::raii::PipelineLayout(mContext->getDevice(), info);
            spdlog::info("Pipeline layout created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline layout: {}", error.what());
            return false;
        }

        return true;
    }

    bool Pipeline::initializeGraphicsPipeline() {
        // Stage infos
        std::vector<vk::PipelineShaderStageCreateInfo> stages;

        // Vertex stage info
        const vk::PipelineShaderStageCreateInfo vertexStage{
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = mShaders[0],
            .pName = "vertexMain"
        };

        stages.push_back(vertexStage);

        // fragment stage info
        const vk::PipelineShaderStageCreateInfo fragmentStage{
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = mShaders[0],
            .pName = "fragmentMain"
        };

        stages.push_back(fragmentStage);

        // Dynamic states
        const std::vector<vk::DynamicState> dynamicStates{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        // Dynamic info
        const vk::PipelineDynamicStateCreateInfo dynamicInfo{
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        // Vertex input info
        const vk::PipelineVertexInputStateCreateInfo vertexInputInfo{

        };

        // Input assembly info
        const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .topology = vk::PrimitiveTopology::eTriangleList
        };

        // Viewport info
        const vk::PipelineViewportStateCreateInfo viewportInfo{
            .viewportCount = 1,
            .scissorCount = 1
        };

        // Rasterization info
        const vk::PipelineRasterizationStateCreateInfo rasterizationInfo{
            .lineWidth = 1.0f
        };

        // Multisample info
        const vk::PipelineMultisampleStateCreateInfo multisampleInfo{
            .rasterizationSamples = vk::SampleCountFlagBits::e1
        };

        // Depth stencil info
        const vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{
            .depthTestEnable = true,
            .depthWriteEnable = true,
            .depthCompareOp = vk::CompareOp::eLess
        };

        // Color blend attachment
        const vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .colorWriteMask =
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA
        };

        // Color blend info
        const vk::PipelineColorBlendStateCreateInfo colorBlendInfo{
            .logicOp = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment
        };

        // Info
        const vk::GraphicsPipelineCreateInfo info{
            .stageCount = static_cast<uint32_t>(stages.size()),
            .pStages = stages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterizationInfo,
            .pMultisampleState = &multisampleInfo,
            .pDepthStencilState = &depthStencilInfo,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicInfo,
            .layout = mLayout
        };

        // Chain
        const vk::StructureChain<
            vk::GraphicsPipelineCreateInfo,
            vk::PipelineRenderingCreateInfo
        > chain{
            info,
            cRenderingInfo
        };

        // Initialize
        try {
            mHandle = vk::raii::Pipeline(mContext->getDevice(), nullptr, chain.get<vk::GraphicsPipelineCreateInfo>());
            spdlog::info("Pipeline created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline: {}", error.what());
            return false;
        }

        return true;
    }

    bool Pipeline::initializeComputePipeline() {

        try {
            // Create pipeline
            //handle = vk::raii::Pipeline(pContext->getDevice(), nullptr, );
            spdlog::info("Pipeline created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline: {}", error.what());
            return false;
        }

        return true;
    }
}
