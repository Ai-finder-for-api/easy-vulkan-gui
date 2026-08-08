// vgui :: X11 input helpers
// Key/mouse events are dispatched in x11_window.cpp poll().
// This TU holds any XInput2 or extended input initialisation.
#if defined(__linux__)
#include <X11/Xlib.h>
namespace vgui { /* XInput2 init would go here */ }
#endif
