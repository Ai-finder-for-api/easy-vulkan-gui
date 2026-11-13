// The Vulkan canvas widget: creation (registered so canvas_window can find its evg window).
#include "qt_common.hpp"
#include <evg/qt/evg_qt.hpp>

namespace evgx {
using namespace qtb;

namespace {
struct CanvasEntry { QWidget* widget; qt::Canvas* canvas; };
constexpr int kMaxCanvases = 32;
CanvasEntry g_canvases[kMaxCanvases];
int g_ncanvas = 0;
}

QtWidget* QtApi::createcanvas(QtWidget* parent, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    if (g_ncanvas >= kMaxCanvases) { evg.seterror("too many canvases"); return nullptr; }
    qt::Canvas* c = new qt::Canvas();
    g_canvases[g_ncanvas++] = CanvasEntry{c, c};
    return place(c, parent, x, y, w, h);
}

// the evg window that a canvas presents into (null for widgets that are not canvases)
Window* QtApi::canvas_window(QtWidget* canvas) {
    for (int i = 0; i < g_ncanvas; i++)
        if (g_canvases[i].widget == W(canvas)) return g_canvases[i].canvas->window();
    return nullptr;
}

}  // namespace evgx
