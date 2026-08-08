#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Canvas                                                 ║
// ║  Root 2-D drawing surface. Owns the UI widget tree.             ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& canvas = vgui::Canvas::active();                       ║
// ║    canvas.clear(vgui::Color::dark_gray());                      ║
// ║    canvas.draw_rect(10, 10, 200, 40, vgui::Color::blue());      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include "font.hpp"
#include "../math/color.hpp"
#include "../math/vec2.hpp"
#include <memory>
#include <string>

namespace vgui {

class Canvas {
public:
    static Canvas& active();

    int   width()  const;
    int   height() const;
    float scale()  const;    ///< DPI scale factor

    // ── Widget factory ────────────────────────────────────
    template<typename T, typename... Args>
    T& make(Args&&... args) {
        auto w = T::create(std::forward<Args>(args)...);
        m_root->add_child(w);
        return *w;
    }

    // ── Immediate-mode primitives ─────────────────────────
    void clear      (Color color = Color::dark_gray());
    void draw_rect  (float x, float y, float w, float h, Color color, float radius = 0);
    void draw_border(float x, float y, float w, float h, Color color, float thickness = 1, float radius = 0);
    void draw_line  (Vec2 a, Vec2 b, Color color, float thickness = 1);
    void draw_circle(Vec2 center, float radius, Color color, int segments = 32);
    void draw_text  (const std::string& text, float x, float y, Color color, float size = 14, Font* font = nullptr);
    void draw_image (class Texture& tex, float x, float y, float w, float h, Color tint = Color::white());

    // ── Update & render ───────────────────────────────────
    void update(float dt);
    void render();

    Widget& root() { return *m_root; }

private:
    Canvas() = default;
    std::shared_ptr<Widget> m_root;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
