#pragma once

#include "mesh.h"
#include "material.h"

namespace render {
    class Renderable {
        const Mesh* mMesh = nullptr;
        const Material* mMaterial = nullptr;
        glm::mat4 mTransform = glm::mat4(1);
    public:
        Renderable(const Mesh* mesh, const Material* material, glm::mat4 transform) :
            mMesh(mesh), mMaterial(material), mTransform(transform) {}

        const Mesh* getMesh() const { return mMesh; }
        const Material* getMaterial() const { return mMaterial; }
        const glm::mat4& getTransform() const { return mTransform; }
    };
}
