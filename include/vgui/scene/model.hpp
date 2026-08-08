#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Model                                                  ║
// ║  Loaded 3-D model: one or more meshes + materials + skeleton.   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../graphics/mesh.hpp"
#include "../graphics/material.hpp"
#include "node.hpp"
#include <string>
#include <vector>
#include <memory>

namespace vgui {

struct SubMesh {
    std::shared_ptr<Mesh>     mesh;
    std::shared_ptr<Material> material;
};

class Model {
public:
    /// Load glTF 2.0 (.glb / .gltf) using built-in parser
    static std::shared_ptr<Model> load(const std::string& path);

    /// Create from existing mesh + material
    static std::shared_ptr<Model> from_mesh(std::shared_ptr<Mesh> mesh,
                                             std::shared_ptr<Material> mat = nullptr);

    const std::string&           name()     const { return m_name; }
    const std::vector<SubMesh>&  submeshes()const { return m_submeshes; }
    Node&                        root_node()       { return *m_root;    }

    void render(class CommandBuffer& cmd, const class Mat4& model_matrix);

private:
    Model() = default;
    std::string              m_name;
    std::vector<SubMesh>     m_submeshes;
    std::shared_ptr<Node>    m_root;
};

} // namespace vgui
