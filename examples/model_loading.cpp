// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui example – Model Loading                                   ║
// ║  Load a glTF model and orbit the camera around it.              ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/vgui.hpp>

int main() {
    auto& win = vgui::make_screen("Model Viewer", 1280, 720);

    // Load a glTF 2.0 model
    auto model = vgui::Model::load("assets/DamagedHelmet.glb");

    // Scene
    auto scene   = vgui::Scene::create("Main");
    auto& entity = scene->create_entity("Helmet");
    entity.add<vgui::Transform>();

    vgui::Camera cam;
    cam.position = {0, 1, 4};
    cam.aspect   = 1280.0f / 720.0f;
    cam.look_at({0, 0, 0});
    scene->active_camera = &cam;

    // Orbit state
    float yaw = 0.0f;

    vgui::Application::get().on_update = [&](float dt) {
        // Mouse drag to orbit
        if (vgui::Mouse::get().is_down(vgui::MouseButton::Left)) {
            yaw += vgui::Mouse::get().delta().x * 0.3f;
        }

        float rad = yaw * 3.14159f / 180.0f;
        cam.position = { std::sin(rad) * 4.0f, 1.0f, std::cos(rad) * 4.0f };
        cam.look_at({0, 0, 0});

        auto& r = vgui::Renderer::get();
        if (!r.begin_frame()) return;
        r.clear(vgui::Color::from_hex(0x1A1A2EFF));
        scene->render();
        r.end_frame();
    };

    vgui::run();
}
