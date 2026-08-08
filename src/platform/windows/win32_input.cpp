// vgui :: Win32 input helpers (raw input, HID)
// Event dispatch is handled inside WndProc in win32_window.cpp.
// This TU holds any additional raw-input or HID initialisation.
#if defined(_WIN32)
#include <windows.h>
namespace vgui {
void win32_init_raw_input() {
    RAWINPUTDEVICE rid[2]{};
    rid[0].usUsagePage = 0x01; rid[0].usUsage = 0x02; // Mouse
    rid[1].usUsagePage = 0x01; rid[1].usUsage = 0x06; // Keyboard
    RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
}
} // namespace vgui
#endif
