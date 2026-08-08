// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: NSWindowImpl  –  Objective-C implementation            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "ns_window.hpp"
#if defined(__APPLE__)

#import  <Cocoa/Cocoa.h>
#include <vgui/input/event_queue.hpp>
#include <vgui/core/logger.hpp>

// ── Delegate ─────────────────────────────────────────────────────────────────
@interface VGUIWindowDelegate : NSObject<NSWindowDelegate>
@property (assign) vgui::NSWindowImpl* impl;
@end

@implementation VGUIWindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    self.impl->close();
    vgui::EventQueue::get().push(vgui::EvClose{});
    return NO;   // We manage lifetime ourselves
}
- (void)windowDidResize:(NSNotification*)n {
    NSWindow* w = n.object;
    NSRect r = [w contentRectForFrameRect:w.frame];
    vgui::EventQueue::get().push(vgui::EvResize{static_cast<int>(r.size.width),
                                                  static_cast<int>(r.size.height)});
}
- (void)windowDidBecomeKey:(NSNotification*) { vgui::EventQueue::get().push(vgui::EvFocus{true});  }
- (void)windowDidResignKey:(NSNotification*) { vgui::EventQueue::get().push(vgui::EvFocus{false}); }
- (void)windowDidMiniaturize:(NSNotification*) {}
- (void)windowDidDeminiaturize:(NSNotification*) {}
@end

// ── NSWindowImpl ──────────────────────────────────────────────────────────────
namespace vgui {

void NSWindowImpl::create(const std::string& title, int w, int h) {
    @autoreleasepool {
        if (!NSApp) {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
            [NSApp finishLaunching];
        }

        m_w = w; m_h = h;
        NSRect frame = NSMakeRect(0, 0, w, h);
        NSWindowStyleMask style = NSWindowStyleMaskTitled
                                | NSWindowStyleMaskClosable
                                | NSWindowStyleMaskMiniaturizable
                                | NSWindowStyleMaskResizable;

        NSWindow* win = [[NSWindow alloc]
            initWithContentRect: frame
                      styleMask: style
                        backing: NSBackingStoreBuffered
                          defer: NO];

        win.title = [NSString stringWithUTF8String:title.c_str()];
        win.acceptsMouseMovedEvents = YES;
        [win center];

        VGUIWindowDelegate* del = [[VGUIWindowDelegate alloc] init];
        del.impl = this;
        win.delegate = del;

        m_nswindow = (__bridge_retained void*)win;
        m_delegate  = (__bridge_retained void*)del;
        m_open = true;
        VGUI_INFO("macOS NSWindow created");
    }
}

void NSWindowImpl::show() {
    @autoreleasepool {
        NSWindow* win = (__bridge NSWindow*)m_nswindow;
        [win makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
    }
}

void NSWindowImpl::hide() {
    @autoreleasepool { [(__bridge NSWindow*)m_nswindow orderOut:nil]; }
}

void NSWindowImpl::close()   { m_open = false; }
void NSWindowImpl::present() {}

void NSWindowImpl::destroy() {
    @autoreleasepool {
        if (m_nswindow) {
            NSWindow* win = (__bridge_transfer NSWindow*)m_nswindow;
            [win close]; m_nswindow = nullptr;
        }
        if (m_delegate) {
            VGUIWindowDelegate* del = (__bridge_transfer VGUIWindowDelegate*)m_delegate;
            (void)del; m_delegate = nullptr;
        }
    }
}

void NSWindowImpl::poll() {
    @autoreleasepool {
        NSEvent* ev;
        while ((ev = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:[NSDate distantPast]
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES])) {
            [NSApp sendEvent:ev];
        }
    }
}

void NSWindowImpl::set_title(const std::string& t) {
    @autoreleasepool {
        (__bridge NSWindow*)m_nswindow).title = [NSString stringWithUTF8String:t.c_str()];
    }
}

void NSWindowImpl::set_size(int w, int h) {
    @autoreleasepool {
        NSWindow* win = (__bridge NSWindow*)m_nswindow;
        NSRect r = win.frame; r.size = NSMakeSize(w, h);
        [win setFrame:r display:YES]; m_w = w; m_h = h;
    }
}

void NSWindowImpl::set_position(int x, int y) {
    @autoreleasepool {
        NSWindow* win = (__bridge NSWindow*)m_nswindow;
        NSScreen* scr = win.screen ?: [NSScreen mainScreen];
        CGFloat sh = scr.frame.size.height;
        [win setFrameTopLeftPoint:NSMakePoint(x, sh - y)];
    }
}

void NSWindowImpl::set_resizable(bool r) {
    @autoreleasepool {
        NSWindow* win = (__bridge NSWindow*)m_nswindow;
        if (r) win.styleMask |=  NSWindowStyleMaskResizable;
        else   win.styleMask &= ~NSWindowStyleMaskResizable;
    }
}

void NSWindowImpl::set_fullscreen(bool f) {
    @autoreleasepool {
        m_fullscreen = f;
        [(__bridge NSWindow*)m_nswindow toggleFullScreen:nil];
    }
}

void NSWindowImpl::set_opacity(float o) {
    @autoreleasepool { (__bridge NSWindow*)m_nswindow).alphaValue = o; }
}

void NSWindowImpl::set_vsync(bool) { /* handled in Metal backend */ }

} // namespace vgui
#endif
