// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui example – Textured Cube                                   ║
// ║  Spinning cube with a texture and a PBR material.               ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/vgui.hpp>

int main() {
    auto& win = vgui::make_screen("Textured Cube", 1280, 720);

    // Geometry
    auto mesh    = vgui::Mesh::make_cube(1.0f);

    // Texture (built-in decoder)
    auto tex     = vgui::Texture::load("assets/crate.png");

    // Material
    auto mat     = vgui::Material::create();
    mat->albedo_map = tex;
    mat->roughness  = 0.6f;
    mat->metallic   = 0.1f;

    // Camera
    vgui::Camera cam;
    cam.position = { 0, 1, 3 };
    cam.aspect   = 1280.0f / 720.0f;
    cam.look_at({0, 0, 0});

    // Per-frame uniform
    struct MVP { vgui::Mat4 model, view, proj; };
    vgui::Uniform<MVP> ubo;

    float angle = 0.0f;

    vgui::Application::get().on_update = [&](float dt) {
        angle += dt * 45.0f;   // 45 degrees/second

        ubo->model = vgui::Mat4::rotate_y(angle * 3.14159f / 180.0f);
        ubo->view  = cam.view_matrix();
        ubo->proj  = cam.proj_matrix();
        ubo.flush();

        auto& r = vgui::Renderer::get();
        if (!r.begin_frame()) return;
        r.clear(vgui::Color::dark_gray());
        // ... bind pipeline, descriptors, draw mesh ...
        r.end_frame();
    };

    vgui::run();
}
