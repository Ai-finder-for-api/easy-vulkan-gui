// vgui :: Wayland window (future)
// Wayland support is planned. Falls back to X11 via XWayland today.
#if defined(__linux__)
namespace vgui { /* wl_display, wl_surface, xdg_toplevel initialisation */ }
#endif
