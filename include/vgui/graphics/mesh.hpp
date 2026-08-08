#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Mesh                                                   ║
// ║  GPU vertex + index buffers with an easy upload API.            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "vertex.hpp"
#include "buffer.hpp"
#include <vector>
#include <memory>
#include <string>

namespace vgui {

class Mesh {
public:
    static std::shared_ptr<Mesh> create(const std::vector<Vertex3D>& verts,
                                         const std::vector<uint32_t>& indices = {});

    static std::shared_ptr<Mesh> create_2d(const std::vector<Vertex2D>& verts,
                                            const std::vector<uint16_t>& indices = {});

    /// Built-in primitive generators
    static std::shared_ptr<Mesh> make_quad    (float w = 1.f, float h = 1.f);
    static std::shared_ptr<Mesh> make_cube    (float size = 1.f);
    static std::shared_ptr<Mesh> make_sphere  (float radius = 1.f, int rings = 32, int segs = 32);
    static std::shared_ptr<Mesh> make_plane   (float w = 10.f, float d = 10.f, int divs = 1);
    static std::shared_ptr<Mesh> make_cylinder(float radius = 0.5f, float height = 1.f, int segs = 32);

    void update_vertices(const std::vector<Vertex3D>& verts);

    Buffer& vertex_buffer();
    Buffer& index_buffer();

    uint32_t vertex_count() const;
    uint32_t index_count()  const;
    bool     has_indices()  const;

private:
    Mesh() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
