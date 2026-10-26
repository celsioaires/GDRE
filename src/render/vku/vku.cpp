#include "vku.h"

#if defined (USE_CPP20_MODULES)
import vulkan_hpp;
#else
#include <vulkan/vulkan_raii.hpp>
#endif

namespace vku {
    bool VKU::initialize() {
        VkInstance i;
        return true;
    }

    void VKU::cleanup() {

    }
}
