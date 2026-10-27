#include "vku.h"

namespace vku {
    bool VKU::initialize(GLFWwindow* windowHandle) {
        if (
            !mContext.initialize(windowHandle) ||
            !mSwapchain.initialize(mContext)
        ) {
            return false;
        }

        return true;
    }
}
