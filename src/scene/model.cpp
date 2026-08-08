#include <vgui/scene/model.hpp>
#include <vgui/core/logger.hpp>
#include <stdexcept>

namespace vgui {

std::shared_ptr<Model> Model::from_mesh(std::shared_ptr<Mesh> mesh,
                                         std::shared_ptr<Material> mat) {
    auto m = std::shared_ptr<Model>(new Model);
    m->m_name = "Mesh";
    m->m_root = Node::create("Root");
    m->m_submeshes.push_back({ mesh, mat });
    return m;
}

std::shared_ptr<Model> Model::load(const std::string& path) {
    // Minimal glTF stub – real parser would go here
    VGUI_WARN("Model::load: full glTF parser not yet implemented, returning empty model");
    auto m = std::shared_ptr<Model>(new Model);
    m->m_name = path;
    m->m_root = Node::create("Root");
    return m;
}

void Model::render(CommandBuffer& cmd, const Mat4& /*model_matrix*/) {
    for (auto& sub : m_submeshes) {
        if (!sub.mesh) continue;
        if (sub.material) sub.material->bind(cmd);
        cmd.bind_vertex(sub.mesh->vertex_buffer());
        if (sub.mesh->has_indices())
            cmd.draw_indexed(sub.mesh->index_count());
        else
            cmd.draw(sub.mesh->vertex_count());
    }
}

} // namespace vgui
