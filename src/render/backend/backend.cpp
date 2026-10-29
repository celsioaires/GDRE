#include "backend.h"

namespace backend {
    bool Backend::initialize(BackendInfo info) {
        if (
            !mContext.initialize(info.window) ||
            !mSwapchain.initialize(mContext, info.width, info.height)
        ) {
            return false;
        }

        // Pipelines
        if (
            // Graphics
            !mGraphicsPipeline.initialize(mContext, PipelineType::GRAPHICS, { "default.spv" })
            // Compute
        ) {
            return false;
        }

        // Command
        if (!mCommand.initialize(mContext, mContext.getGraphicsFamily())) {
            return false;
        }

        return true;
    }
}
