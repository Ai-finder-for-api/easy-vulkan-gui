// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: MonitorManager  –  implementation                      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/window/monitor.hpp>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__APPLE__)
#  include <CoreGraphics/CoreGraphics.h>
#elif defined(__linux__)
#  include <X11/Xlib.h>
#  include <X11/extensions/Xrandr.h>
#endif

namespace vgui {

MonitorManager& MonitorManager::get() { static MonitorManager s; return s; }

const std::vector<Monitor>& MonitorManager::all()     const { return m_monitors; }
const Monitor&              MonitorManager::primary()  const {
    for (auto& m : m_monitors) if (m.is_primary) return m;
    return m_monitors.front();
}

void MonitorManager::refresh() {
    m_monitors.clear();

#if defined(_WIN32)
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hm, HDC, LPRECT, LPARAM lp) -> BOOL {
        auto& mgr = *reinterpret_cast<MonitorManager*>(lp);
        MONITORINFOEXA info{}; info.cbSize = sizeof(info);
        GetMonitorInfoA(hm, &info);

        DEVMODEA dm{}; dm.dmSize = sizeof(dm);
        EnumDisplaySettingsA(info.szDevice, ENUM_CURRENT_SETTINGS, &dm);

        Monitor m;
        m.name        = info.szDevice;
        m.position    = { static_cast<float>(info.rcMonitor.left),
                          static_cast<float>(info.rcMonitor.top) };
        m.is_primary  = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
        m.current_mode= { dm.dmPelsWidth, dm.dmPelsHeight,
                          static_cast<int>(dm.dmDisplayFrequency) };
        m.native_handle = hm;
        mgr.m_monitors.push_back(m);
        return TRUE;
    }, reinterpret_cast<LPARAM>(this));

#elif defined(__APPLE__)
    uint32_t count = 0;
    CGGetActiveDisplayList(0, nullptr, &count);
    std::vector<CGDirectDisplayID> ids(count);
    CGGetActiveDisplayList(count, ids.data(), &count);
    for (auto id : ids) {
        Monitor m;
        auto bounds = CGDisplayBounds(id);
        m.position   = { static_cast<float>(bounds.origin.x), static_cast<float>(bounds.origin.y) };
        m.is_primary = (id == CGMainDisplayID());
        auto mode    = CGDisplayCopyDisplayMode(id);
        m.current_mode = {
            static_cast<int>(CGDisplayModeGetWidth(mode)),
            static_cast<int>(CGDisplayModeGetHeight(mode)),
            static_cast<int>(CGDisplayModeGetRefreshRate(mode))
        };
        CGDisplayModeRelease(mode);
        m.native_handle = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
        m_monitors.push_back(m);
    }

#elif defined(__linux__)
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return;
    int screen = DefaultScreen(dpy);
    Monitor m;
    m.is_primary       = true;
    m.current_mode     = { DisplayWidth(dpy, screen), DisplayHeight(dpy, screen), 60 };
    m.native_handle    = dpy;
    m_monitors.push_back(m);
    XCloseDisplay(dpy);
#endif
}

} // namespace vgui
