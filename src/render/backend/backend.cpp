#include "backend.h"

namespace backend {
    bool Backend::initialize(const WSI& wsi) {
        if (
            !mContext.initialize(wsi.window) ||
            !mSwapchain.initialize(mContext, wsi.width, wsi.height)
        ) {
            return false;
        }

        // Pipelines
        if (
            // Graphics
            !mGraphicsPipeline.initialize(mContext, { "default.spv" })
            // Compute
        ) {
            return false;
        }

        return true;
    }
}
