#include "material.h"

namespace render {
    bool Material::initialize(const backend::Backend& backend) {
        mBackend = &backend;

        const backend::Context& context = mBackend->getContext();

        // Descriptor set
        const backend::Descriptor& descriptor = mBackend->getDefaultDescriptor();
        const backend::DescriptorLayout& descriptorLayout = mBackend->getMaterialDescriptorLayout() ;

        if (!mDescriptorSet.initialize(
            context,
            descriptor.getPool(),
            descriptorLayout.getHandle()
        )) {
            return false;
        }

        return true;
    }
}
