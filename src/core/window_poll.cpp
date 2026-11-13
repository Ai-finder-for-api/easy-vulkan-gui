// Window enumeration and the event queue as seen by the application (poll_event, poll_close, focus).
#include "state.hpp"
#include "platform.hpp"

namespace evgx {
using namespace st;

bool Api::poll_close(Window*& which) {
    plat::pump();
    for (int i = 0; i < kMaxWindows; i++)
        if (g_used[i] && g_pool[i].closed && !g_pool[i].close_reported) {
            g_pool[i].close_reported = true;
            which = &g_pool[i];
            return true;
        }
    return false;
}

bool Api::poll_event(Event& out) {
    if (g_qn > 0) {
        out = g_q[g_qh];
        g_qh = (g_qh + 1) % kQueue;
        g_qn--;
        return true;
    }
    Window* w = nullptr;
    if (poll_close(w)) { out = Event(); out.type = EventType::Quit; out.window = w; return true; }
    return false;
}

int Api::window_count() const {
    int n = 0;
    for (int i = 0; i < kMaxWindows; i++) if (g_used[i]) n++;
    return n;
}

Window* Api::window_at(int i) const {
    for (int k = 0; k < kMaxWindows; k++)
        if (g_used[k]) { if (i == 0) return &g_pool[k]; i--; }
    return nullptr;
}

Window* Api::focus_window() const { return g_focus; }

}  // namespace evgx
