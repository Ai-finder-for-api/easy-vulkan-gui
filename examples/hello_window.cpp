// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui example – Hello Window                                    ║
// ║  The simplest possible vgui application.                        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/vgui.hpp>

int main() {
    // ── Create a 1280×720 window ──────────────────────────────────
    auto& win   = vgui::make_screen("Hello vgui", 1280, 720);
    win.set_vsync(true);

    // ── A label in the centre ─────────────────────────────────────
    auto& lbl   = vgui::make_label("Hello, vgui!");
    lbl.x       = 540;
    lbl.y       = 340;
    lbl.color   = vgui::Color::white();
    lbl.font_size = 24.0f;

    // ── A quit button ─────────────────────────────────────────────
    auto& btn   = vgui::make_button("Quit");
    btn.x       = 580;
    btn.y       = 400;
    btn.width   = 120;
    btn.height  = 36;
    btn.on_click = []{ vgui::quit(); };

    // ── Per-frame update ──────────────────────────────────────────
    vgui::Application::get().on_update = [](float dt) {
        // Press Escape to exit
        if (vgui::Keyboard::get().pressed(vgui::Key::Escape))
            vgui::quit();
    };

    vgui::run();
}
