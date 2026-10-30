#include "pipeline.h"

#include <fstream>

#include <spdlog/spdlog.h>

namespace backend {
    bool Pipeline::initialize(const Context& context, PipelineType type, const std::vector<std::string>& files) {
        sContext = &context;

        // Shaders
        mShaders = initializeShaders(files);

        // Layout
        mLayout = initializeLayout();

        if (mLayout == nullptr) {
            return false;
        }

        // Handle
        switch (type) {
            case PipelineType::GRAPHICS: {
                mHandle = initializeGraphicsPipeline(mShaders, mLayout);
                break;
            }
            case PipelineType::COMPUTE: {
                mHandle = initializeComputePipeline();
                break;
            }
        }

        if (mHandle == nullptr) {
            return false;
        }

        return true;
    }

    std::vector<vk::raii::ShaderModule> Pipeline::initializeShaders(const std::vector<std::string> &files) {
        std::vector<vk::raii::ShaderModule> shaders;

        for (const std::string& file : files) {
            // Open stream
            std::ifstream stream("shaders/compiled/" + file, std::ios::ate | std::ios::binary);

            if (!stream.is_open()) {
                // Failed
                spdlog::error("Failed to open shader: {}", file);
                return shaders;
            }

            // Code
            std::vector<char> code(stream.tellg());

            stream.seekg(0, std::ios::beg);
            stream.read(code.data(), static_cast<std::streamsize>(code.size()));

            // Info
            vk::ShaderModuleCreateInfo createInfo{
                .codeSize = code.size() * sizeof(char),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
            };

            vk::raii::ShaderModule shader = nullptr;

            try {
                // Create shader
                shader = vk::raii::ShaderModule(sContext->getDevice(), createInfo);
                spdlog::info("Shader created successfully: {}", file);
            }
            catch (const vk::SystemError& error) {
                // Failed
                spdlog::error("Failed to create shader: {}\n{}", file, error.what());
                return shaders;
            }

            shaders.push_back(std::move(shader));
        }

        return shaders;
    }

    vk::raii::PipelineLayout Pipeline::initializeLayout() {
        // Layout info
        vk::PipelineLayoutCreateInfo layoutInfo{

        };

        vk::raii::PipelineLayout handle = nullptr;

        try {
            // Create layout
            handle = vk::raii::PipelineLayout(sContext->getDevice(), layoutInfo);
            spdlog::info("Pipeline layout created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline layout: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::Pipeline Pipeline::initializeGraphicsPipeline(const std::vector<vk::raii::ShaderModule>& shaders, const vk::raii::PipelineLayout& layout) {
        // Stage infos
        std::vector<vk::PipelineShaderStageCreateInfo> stages;

        // Vertex stage info
        vk::PipelineShaderStageCreateInfo vertexStage{
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = shaders[0],
            .pName = "vertexMain"
        };

        stages.push_back(vertexStage);

        // fragment stage info
        vk::PipelineShaderStageCreateInfo fragmentStage{
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = shaders[0],
            .pName = "fragmentMain"
        };

        stages.push_back(fragmentStage);

        // Dynamic states
        std::vector<vk::DynamicState> dynamicStates{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        // Dynamic info
        vk::PipelineDynamicStateCreateInfo dynamicInfo{
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        // Vertex input info
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{

        };

        // Input assembly info
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .topology = vk::PrimitiveTopology::eTriangleList
        };

        // Viewport info
        vk::PipelineViewportStateCreateInfo viewportInfo{
            .viewportCount = 1,
            .scissorCount = 1
        };

        // Rasterization info
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{
            .lineWidth = 1.0f
        };

        // Multisample info
        vk::PipelineMultisampleStateCreateInfo multisampleInfo{
            .rasterizationSamples = vk::SampleCountFlagBits::e1
        };

        // Depth stencil info
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{

        };

        // Color blend attachment
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .colorWriteMask =
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA
        };

        // Color blend info
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{
            .logicOp = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment
        };

        // Rendering info
        vk::Format colorAttachmentFormat = vk::Format::eB8G8R8A8Srgb;

        vk::PipelineRenderingCreateInfo renderingInfo{
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &colorAttachmentFormat
        };

        // Info
        vk::GraphicsPipelineCreateInfo createInfo{
            .stageCount = static_cast<uint32_t>(stages.size()),
            .pStages = stages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterizationInfo,
            .pMultisampleState = &multisampleInfo,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicInfo,
            .layout = layout
        };

        // Chain
        vk::StructureChain<
            vk::GraphicsPipelineCreateInfo,
            vk::PipelineRenderingCreateInfo
        > createChain{
            createInfo,
            renderingInfo
        };

        vk::raii::Pipeline handle = nullptr;

        try {
            // Create pipeline
            handle = vk::raii::Pipeline(sContext->getDevice(), nullptr, createChain.get<vk::GraphicsPipelineCreateInfo>());
            spdlog::info("Pipeline created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline: {}", error.what());
            return nullptr;
        }

        return handle;
    }

    vk::raii::Pipeline Pipeline::initializeComputePipeline() {
        vk::raii::Pipeline handle = nullptr;

        try {
            // Create pipeline
            //handle = vk::raii::Pipeline(pContext->getDevice(), nullptr, );
            spdlog::info("Pipeline created successfully");
        }
        catch (const vk::SystemError& error) {
            // Failed
            spdlog::error("Failed to create pipeline: {}", error.what());
            return nullptr;
        }

        return handle;
    }
}
