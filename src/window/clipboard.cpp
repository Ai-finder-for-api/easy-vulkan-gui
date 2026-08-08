// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Clipboard  –  implementation                           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/window/clipboard.hpp>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__APPLE__)
#  include <objc/runtime.h>
#  include <objc/message.h>
#elif defined(__linux__)
#  include <X11/Xlib.h>
#  include <X11/Xatom.h>
#endif

namespace vgui {

Clipboard& Clipboard::get() { static Clipboard s; return s; }

#if defined(_WIN32)

std::string Clipboard::get_text() const {
    if (!OpenClipboard(nullptr)) return {};
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (!h) { CloseClipboard(); return {}; }
    auto* w = static_cast<const wchar_t*>(GlobalLock(h));
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    std::string s(n - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), n, nullptr, nullptr);
    GlobalUnlock(h);
    CloseClipboard();
    return s;
}

void Clipboard::set_text(const std::string& s) {
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    HANDLE h = GlobalAlloc(GMEM_MOVEABLE, n * sizeof(wchar_t));
    auto* w  = static_cast<wchar_t*>(GlobalLock(h));
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, w, n);
    GlobalUnlock(h);
    if (OpenClipboard(nullptr)) { EmptyClipboard(); SetClipboardData(CF_UNICODETEXT, h); CloseClipboard(); }
}

bool Clipboard::has_text() const {
    return IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
}

void Clipboard::clear() {
    if (OpenClipboard(nullptr)) { EmptyClipboard(); CloseClipboard(); }
}

#elif defined(__APPLE__)

// Thin ObjC bridge without importing full Cocoa headers
std::string Clipboard::get_text() const {
    id pb   = ((id(*)(Class, SEL))objc_msgSend)(objc_getClass("NSPasteboard"),
               sel_registerName("generalPasteboard"));
    id type = ((id(*)(id, SEL, id))objc_msgSend)(pb,
               sel_registerName("availableTypeFromArray:"),
               ((id(*)(Class, SEL, const id*, NSUInteger))objc_msgSend)(
                   objc_getClass("NSArray"), sel_registerName("arrayWithObject:"),
                   (id)@"public.utf8-plain-text", 1));
    if (!type) return {};
    id str  = ((id(*)(id, SEL, id))objc_msgSend)(pb,
               sel_registerName("stringForType:"), type);
    if (!str) return {};
    return ((const char*(*)(id, SEL))objc_msgSend)(str, sel_registerName("UTF8String"));
}

void Clipboard::set_text(const std::string& s) {
    id pb  = ((id(*)(Class, SEL))objc_msgSend)(objc_getClass("NSPasteboard"),
              sel_registerName("generalPasteboard"));
    ((void(*)(id, SEL))objc_msgSend)(pb, sel_registerName("clearContents"));
    id str = ((id(*)(Class, SEL, const char*))objc_msgSend)(
                 objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), s.c_str());
    ((void(*)(id, SEL, id, id))objc_msgSend)(pb, sel_registerName("setString:forType:"),
        str, (id)@"public.utf8-plain-text");
}

bool Clipboard::has_text() const { return !get_text().empty(); }
void Clipboard::clear() {
    id pb = ((id(*)(Class, SEL))objc_msgSend)(objc_getClass("NSPasteboard"),
             sel_registerName("generalPasteboard"));
    ((void(*)(id, SEL))objc_msgSend)(pb, sel_registerName("clearContents"));
}

#else

// Linux X11 clipboard (simplified; full impl requires event loop selection)
std::string Clipboard::get_text() const { return {}; /* TODO: X11 selection */ }
void Clipboard::set_text(const std::string&) {}
bool Clipboard::has_text() const { return false; }
void Clipboard::clear() {}

#endif

} // namespace vgui
