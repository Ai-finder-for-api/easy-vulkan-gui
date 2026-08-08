#include <vgui/graphics/mesh.hpp>
#include <cmath>

namespace vgui {

struct Mesh::Impl {
    std::shared_ptr<Buffer> vbo, ibo;
    uint32_t vert_count = 0, idx_count = 0;
    bool has_idx = false;
};

std::shared_ptr<Mesh> Mesh::create(const std::vector<Vertex3D>& verts,
                                    const std::vector<uint32_t>& indices) {
    auto m = std::shared_ptr<Mesh>(new Mesh);
    m->m_impl = std::make_unique<Impl>();
    m->m_impl->vbo = Buffer::create(BufferUsage::Vertex,
                                     verts.size() * sizeof(Vertex3D), verts.data());
    m->m_impl->vert_count = static_cast<uint32_t>(verts.size());
    if (!indices.empty()) {
        m->m_impl->ibo = Buffer::create(BufferUsage::Index,
                                         indices.size() * sizeof(uint32_t), indices.data());
        m->m_impl->idx_count = static_cast<uint32_t>(indices.size());
        m->m_impl->has_idx = true;
    }
    return m;
}

std::shared_ptr<Mesh> Mesh::make_quad(float w, float h) {
    float hw = w * 0.5f, hh = h * 0.5f;
    std::vector<Vertex3D> v = {
        {{ -hw,-hh,0 }, {0,0,1}, {0,1} },
        {{  hw,-hh,0 }, {0,0,1}, {1,1} },
        {{  hw, hh,0 }, {0,0,1}, {1,0} },
        {{ -hw, hh,0 }, {0,0,1}, {0,0} },
    };
    std::vector<uint32_t> i = { 0,1,2, 0,2,3 };
    return create(v, i);
}

std::shared_ptr<Mesh> Mesh::make_cube(float s) {
    float h = s * 0.5f;
    std::vector<Vertex3D> v;
    auto face = [&](Vec3 n, Vec3 r, Vec3 u) {
        Vec3 c = n * h;
        v.push_back({ c - r*h - u*h, n, {0,1} });
        v.push_back({ c + r*h - u*h, n, {1,1} });
        v.push_back({ c + r*h + u*h, n, {1,0} });
        v.push_back({ c - r*h + u*h, n, {0,0} });
    };
    face({ 0,0, 1},{1,0,0},{0,1,0}); face({ 0,0,-1},{-1,0,0},{0,1,0});
    face({ 1,0, 0},{0,0,-1},{0,1,0}); face({-1,0, 0},{0,0, 1},{0,1,0});
    face({ 0,1, 0},{1,0,0},{0,0,-1}); face({ 0,-1,0},{1,0,0},{0,0, 1});
    std::vector<uint32_t> idx;
    for (uint32_t f = 0; f < 6; ++f) {
        uint32_t b = f*4;
        idx.insert(idx.end(), {b,b+1,b+2, b,b+2,b+3});
    }
    return create(v, idx);
}

std::shared_ptr<Mesh> Mesh::make_sphere(float radius, int rings, int segs) {
    std::vector<Vertex3D> v;
    std::vector<uint32_t> idx;
    for (int r = 0; r <= rings; ++r) {
        float phi = 3.14159265f * r / rings;
        for (int s = 0; s <= segs; ++s) {
            float theta = 2.0f * 3.14159265f * s / segs;
            Vec3 n { std::sin(phi)*std::cos(theta), std::cos(phi), std::sin(phi)*std::sin(theta) };
            v.push_back({ n * radius, n, { (float)s/segs, (float)r/rings } });
        }
    }
    for (int r = 0; r < rings; ++r)
        for (int s = 0; s < segs; ++s) {
            int a = r*(segs+1)+s, b = a+1, c = a+(segs+1), d = c+1;
            idx.insert(idx.end(), { (uint32_t)a,(uint32_t)b,(uint32_t)d,
                                    (uint32_t)a,(uint32_t)d,(uint32_t)c });
        }
    return create(v, idx);
}

std::shared_ptr<Mesh> Mesh::make_plane(float w, float d, int divs) {
    std::vector<Vertex3D> v;
    std::vector<uint32_t> idx;
    float hw = w*0.5f, hd = d*0.5f;
    int n = divs + 1;
    for (int z = 0; z < n; ++z)
        for (int x = 0; x < n; ++x) {
            float fx = (float)x/divs, fz = (float)z/divs;
            v.push_back({{ -hw+fx*w, 0, -hd+fz*d }, {0,1,0}, {fx, fz} });
        }
    for (int z = 0; z < divs; ++z)
        for (int x = 0; x < divs; ++x) {
            uint32_t a = z*n+x; uint32_t b = a+1; uint32_t c = a+n; uint32_t d2 = c+1;
            idx.insert(idx.end(), {a,c,b, b,c,d2});
        }
    return create(v, idx);
}

Buffer& Mesh::vertex_buffer()  { return *m_impl->vbo; }
Buffer& Mesh::index_buffer()   { return *m_impl->ibo; }
uint32_t Mesh::vertex_count()  const { return m_impl->vert_count; }
uint32_t Mesh::index_count()   const { return m_impl->idx_count; }
bool     Mesh::has_indices()   const { return m_impl->has_idx; }

void Mesh::update_vertices(const std::vector<Vertex3D>& verts) {
    m_impl->vbo->upload(verts.data(), verts.size() * sizeof(Vertex3D));
    m_impl->vert_count = static_cast<uint32_t>(verts.size());
}

} // namespace vgui
