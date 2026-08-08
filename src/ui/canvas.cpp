#include <vgui/ui/canvas.hpp>
#include <vgui/ui/widget.hpp>

namespace vgui {

struct Canvas::Impl {
    int   width  = 1280;
    int   height = 720;
    float scale  = 1.0f;
};

Canvas& Canvas::active() { static Canvas s; if (!s.m_impl) { s.m_impl = std::make_unique<Impl>(); s.m_root = Widget::create(); } return s; }

int   Canvas::width()  const { return m_impl->width;  }
int   Canvas::height() const { return m_impl->height; }
float Canvas::scale()  const { return m_impl->scale;  }

void Canvas::clear      (Color)                         {}
void Canvas::draw_rect  (float,float,float,float,Color,float) {}
void Canvas::draw_border(float,float,float,float,Color,float,float) {}
void Canvas::draw_line  (Vec2,Vec2,Color,float)         {}
void Canvas::draw_circle(Vec2,float,Color,int)          {}
void Canvas::draw_text  (const std::string&,float,float,Color,float,Font*) {}
void Canvas::draw_image (class Texture&,float,float,float,float,Color) {}

void Canvas::update(float dt) { if (m_root) m_root->update(dt); }
void Canvas::render()        { if (m_root) m_root->draw();   }

} // namespace vgui
