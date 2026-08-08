// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui example – UI Demo                                         ║
// ║  Shows buttons, labels, textboxes and layout.                   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/vgui.hpp>
#include <string>

int main() {
    vgui::AppConfig cfg;
    cfg.title  = "vgui UI Demo";
    cfg.width  = 900;
    cfg.height = 600;
    vgui::Application::get().init(cfg);

    auto& win = vgui::make_screen(cfg.title, cfg.width, cfg.height);

    // ── Vertical layout container ──────────────────────────────────
    auto& col   = vgui::make_layout(vgui::LayoutType::VStack);
    col.x       = 40;
    col.y       = 40;
    col.width   = 300;
    col.height  = 500;
    col.gap     = 12;

    // ── Title label ───────────────────────────────────────────────
    auto& title = vgui::make_label("vgui UI Demo");
    title.font_size = 20.0f;
    title.color     = vgui::Color::white();
    col.add_child(std::shared_ptr<vgui::Widget>(&title, [](auto*){}));

    // ── Name input ────────────────────────────────────────────────
    auto& tb   = vgui::make_textbox("Enter your name...");
    tb.width   = 280;
    tb.height  = 32;
    col.add_child(std::shared_ptr<vgui::Widget>(&tb, [](auto*){}));

    // ── Status label (updated on submit) ─────────────────────────
    auto& status = vgui::make_label("Type a name and press Enter.");
    status.color  = vgui::Color::light_gray();
    tb.on_submit  = [&](const std::string& name) {
        status.text = "Hello, " + name + "!";
    };
    col.add_child(std::shared_ptr<vgui::Widget>(&status, [](auto*){}));

    // ── Row of colour buttons ─────────────────────────────────────
    auto& row  = vgui::make_layout(vgui::LayoutType::HStack);
    row.width  = 280; row.height = 40; row.gap = 8;

    auto make_color_btn = [&](const std::string& label, vgui::Color col_c) -> vgui::Button& {
        auto& b = vgui::make_button(label);
        b.width = 80; b.height = 32;
        b.normal_color = col_c;
        b.hover_color  = col_c.lerp(vgui::Color::white(), 0.15f);
        b.press_color  = col_c.lerp(vgui::Color::black(), 0.2f);
        return b;
    };

    auto& bRed   = make_color_btn("Red",   vgui::Color::from_hex(0xC0392BFF));
    auto& bGreen = make_color_btn("Green", vgui::Color::from_hex(0x27AE60FF));
    auto& bBlue  = make_color_btn("Blue",  vgui::Color::from_hex(0x2980B9FF));
    (void)bRed; (void)bGreen; (void)bBlue;

    // ── Quit button ───────────────────────────────────────────────
    auto& qbtn   = vgui::make_button("Quit");
    qbtn.width   = 100; qbtn.height = 34;
    qbtn.on_click = []{ vgui::quit(); };
    col.add_child(std::shared_ptr<vgui::Widget>(&qbtn, [](auto*){}));

    vgui::run();
}
