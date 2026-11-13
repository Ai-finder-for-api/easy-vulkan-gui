// evg: easy Vulkan GUI.
//   auto* w = evg.createwindow(1280, 720, "viewer");
//   auto* m = evg.loadglb("model.glb");
//   while (w->open()) { evg.begin(w, .1f,.1f,.15f); evg.camera(cam); evg.draw(m); evg.end(); }
#pragma once
#include <stdint.h>
#include <stddef.h>
#include "math.hpp"
#include "scene.hpp"
#include "lighting.hpp"

namespace evgx {

namespace key {  // non-letter key codes (letters/digits use their uppercase ASCII: w->key('W'))
constexpr int Tab = 9, Enter = 13, Shift = 16, Ctrl = 17, Esc = 27, Space = 32,
              Left = 37, Up = 38, Right = 39, Down = 40;
}

struct Window;

// Event queue entry (filled by the OS backend and by Window::on_* injection; read with evg.poll_event).
enum class EventType : int { None = 0, Quit, KeyDown, KeyUp, MouseMotion, MouseDown, MouseUp, MouseWheel, Resize };
struct Event {
    EventType type = EventType::None;
    Window* window = nullptr;
    int key = 0;                  // KeyDown/KeyUp: evg key code (uppercase ASCII letters/digits, or key::*)
    bool repeat = false;
    int x = 0, y = 0;             // mouse position (MouseMotion/MouseDown/MouseUp)
    int dx = 0, dy = 0;           // MouseMotion delta
    int button = 0;               // MouseDown/MouseUp: 0 left, 1 right, 2 middle
    float wheel = 0;              // MouseWheel notches
    int width = 0, height = 0;    // Resize
};

struct Window {
    int  width = 0, height = 0;            // client size in pixels
    bool closed = true;
    bool close_reported = false;
    void* native = nullptr;                // HWND on Windows, null for headless windows
    bool  external = false;                // true for windows owned by someone else (attach): evg never destroys them

    // ---- input (valid after open()) ----
    uint8_t keys[256] = {};                // currently held
    uint8_t pressed[256] = {};             // went down this frame
    bool    buttons[3] = {};               // 0 left, 1 right, 2 middle
    int     mouse_x = 0, mouse_y = 0;
    float   dx = 0, dy = 0;                // mouse movement this frame
    float   wheel = 0;                     // wheel notches this frame
    float   dt = 0;                        // seconds since the previous open()
    bool    had_mouse = false;
    double  last_time = -1;

    // ---- software framebuffer (BGRA, top-down) ----
    uint32_t* color = nullptr;
    float*    depth = nullptr;
    int       fbw = 0, fbh = 0;

    bool open();                           // pump OS events, update input and dt; false once closed
    bool key_down(int k) const    { return k >= 0 && k < 256 && keys[k]; }
    bool key_pressed(int k) const { return k >= 0 && k < 256 && pressed[k]; }
    bool button(int b) const      { return b >= 0 && b < 3 && buttons[b]; }

    // event entry points (used by the OS backend; also handy for tests / scripted input)
    void on_key(int k, bool down, bool repeat = false);
    void on_mouse_move(int x, int y);
    void on_button(int b, bool down);
    void on_wheel(float notches);
    void on_resize(int w, int h);
};

struct Camera {                            // yaw 0 looks down -Z; +yaw turns right; +pitch looks up
    math::vec3<float> pos = math::vec3<float>(0.f, 0.f, 5.f);
    float yaw = 0.f, pitch = 0.f;
    float fov = 1.0471975512f, znear = 0.1f, zfar = 1000.f;

    math::vec3<float> forward() const {
        float cp = math::cos(pitch);
        return math::vec3<float>(math::sin(yaw) * cp, math::sin(pitch), -math::cos(yaw) * cp);
    }
    math::vec3<float> right() const { return math::normalize(math::cross(forward(), math::vec3<float>(0.f, 1.f, 0.f))); }
    math::mat4 view() const { return math::lookAt(pos, pos + forward(), math::vec3<float>(0.f, 1.f, 0.f)); }
    math::mat4 proj(float aspect) const { return math::perspective(fov, aspect, znear, zfar); }
    void fly(const Window& w, float speed = 3.f);   // WASD + Q/E (or Ctrl/Space), Shift = x3, drag right mouse to look
};

struct Orbit {                             // drag left mouse to orbit, wheel to zoom
    math::vec3<float> target = math::vec3<float>(0.f, 0.f, 0.f);
    float dist = 5.f, yaw = 0.f, pitch = 0.3f;
    void update(const Window& w);
    void frame(const Model* m);            // aim at the model's bounds
    Camera camera() const;
};

// ---- Qt 6 widgets: evg.qt.createbutton(...) and friends ----
// Real when built with -DEVG_WITH_QT=ON; otherwise every call fails and error() says so.
// Widgets are created in a parent's coordinate space: pass a window from createwindow, or a parent widget, or null.
struct QtWidget;                                      // opaque Qt widget handle
struct QtPainter;                                     // opaque painter; valid only inside a paint callback
// paint callback: draws the widget's contents; w and h are the widget's current size
typedef void (*QtPaintFn)(void* user, QtPainter* p, int w, int h);
struct QtApi {
    bool available() const;                           // Qt support was compiled in
    bool init();                                      // create the Qt application (once)
    bool pump();                                      // process Qt events; false once no Qt window is visible
    void quit();                                      // close every Qt top-level window
    QtWidget* createwindow(const char* title, int w, int h);
    QtWidget* createbutton(QtWidget* parent, const char* text, int x, int y, int w, int h);
    QtWidget* createlabel(QtWidget* parent, const char* text, int x, int y, int w, int h);
    QtWidget* createcheckbox(QtWidget* parent, const char* text, int x, int y, int w, int h);
    QtWidget* createslider(QtWidget* parent, int min, int max, int x, int y, int w, int h);
    QtWidget* createlineedit(QtWidget* parent, const char* text, int x, int y, int w, int h);
    QtWidget* createcanvas(QtWidget* parent, int x, int y, int w, int h);   // Vulkan-presented area; see canvas_window
    Window*   canvas_window(QtWidget* canvas);        // the evg window drawn by a createcanvas area
    void setclick(QtWidget* widget, void (*fn)(void*), void* user);         // buttons: clicked
    void setchange(QtWidget* widget, void (*fn)(void*), void* user);        // checkbox toggled, slider moved, text edited
    void settext(QtWidget* widget, const char* text);                       // labels, buttons, edits, window titles
    const char* gettext(QtWidget* widget);            // valid until the next gettext call
    void setvalue(QtWidget* widget, int value);       // sliders
    int  getvalue(QtWidget* widget);
    void setchecked(QtWidget* widget, bool on);       // checkboxes
    bool getchecked(QtWidget* widget);
    void setenabled(QtWidget* widget, bool on);
    void setgeometry(QtWidget* widget, int x, int y, int w, int h);
    void show(QtWidget* widget);
    void hide(QtWidget* widget);
    void click(QtWidget* button);                     // synthetic click (tests, automation)
    void close(QtWidget* widget);
    bool isvisible(QtWidget* widget);

    // ---- layouts: a box decides its children's geometry (addtobox); windows take a central widget ----
    QtWidget* createbox(QtWidget* parent, bool vertical, int x, int y, int w, int h);
    void addtobox(QtWidget* box, QtWidget* child);
    void setcentral(QtWidget* window, QtWidget* widget);
    void setspacing(QtWidget* box, int px);
    void setmargins(QtWidget* box, int px);

    // ---- menus and actions (windows created by createwindow have a menu bar) ----
    QtWidget* addmenu(QtWidget* window, const char* title);
    bool addaction(QtWidget* menu, const char* text, const char* shortcut, void (*fn)(void*), void* user);  // shortcut e.g. "Ctrl+S"
    void addseparator(QtWidget* menu);
    bool triggeraction(QtWidget* menu, const char* text);   // activate an action by its text (automation, tests)

    // ---- dialogs (modal, synchronous) ----
    int  messagebox(QtWidget* parent, const char* title, const char* text, int kind);   // kind 0 info, 1 warning, 2 yes/no: returns 1 yes, 0 no
    const char* openfile(QtWidget* parent, const char* title, const char* filter);      // "" when cancelled; filter like "Images (*.png *.jpg)"
    const char* savefile(QtWidget* parent, const char* title, const char* filter);

    // ---- timers (repeating; returns an id > 0, 0 on failure) ----
    int  createtimer(int ms, void (*fn)(void*), void* user);
    void stoptimer(int id);

    // ---- settings, persisted with QSettings ----
    void setapp(const char* organization, const char* application);   // where settings live; default "evg" / "evg-app"
    void setsetting(const char* key, const char* value);
    const char* getsetting(const char* key, const char* fallback);    // valid until the next getsetting call

    // ---- background work: fn(user) runs on a worker thread, then done(duser) runs on the GUI thread ----
    bool runasync(void (*fn)(void*), void* user, void (*done)(void*), void* duser);

    // ---- table views ----
    QtWidget* createtable(QtWidget* parent, int rows, int cols, int x, int y, int w, int h);
    void settablecell(QtWidget* table, int row, int col, const char* text);
    const char* gettablecell(QtWidget* table, int row, int col);       // valid until the next gettablecell call
    void setheader(QtWidget* table, int col, const char* text);

    // ---- style ----
    void setstylesheet(QtWidget* widget, const char* css);           // Qt style sheets (CSS-like)

    // ---- more inputs ----
    QtWidget* createradio(QtWidget* parent, const char* text, int x, int y, int w, int h);       // exclusive within a parent
    QtWidget* createspinbox(QtWidget* parent, int min, int max, int x, int y, int w, int h);     // integer; setvalue/getvalue
    QtWidget* createdoublespinbox(QtWidget* parent, double min, double max, int decimals, int x, int y, int w, int h);
    void setdoublevalue(QtWidget* widget, double value);
    double getdoublevalue(QtWidget* widget);
    QtWidget* createprogress(QtWidget* parent, int min, int max, int x, int y, int w, int h);    // setvalue/getvalue
    QtWidget* createtextedit(QtWidget* parent, const char* text, int x, int y, int w, int h);    // multi-line plain text
    void setreadonly(QtWidget* widget, bool on);      // line edits and text edits

    // ---- item lists: combo boxes and list views share one API ----
    QtWidget* createcombo(QtWidget* parent, int x, int y, int w, int h);
    QtWidget* createlist(QtWidget* parent, int x, int y, int w, int h);
    void additem(QtWidget* widget, const char* text);
    void clearitems(QtWidget* widget);
    void setcurrentindex(QtWidget* widget, int index);   // -1 clears the selection
    int  getcurrentindex(QtWidget* widget);              // -1 when nothing is selected
    const char* getcurrenttext(QtWidget* widget);        // "" when nothing is selected; valid until the next call

    // ---- containers ----
    QtWidget* creategroup(QtWidget* parent, const char* title, int x, int y, int w, int h);   // titled frame; children go inside
    QtWidget* createtabs(QtWidget* parent, int x, int y, int w, int h);
    QtWidget* addtab(QtWidget* tabs, const char* title);  // returns the page widget; create children with it as parent
    void setstatus(QtWidget* window, const char* text);   // status bar of a createwindow window

    // ---- custom painting: the callback draws with the setpaint/fill/stroke/draw functions below ----
    QtWidget* createpaint(QtWidget* parent, QtPaintFn fn, void* user, int x, int y, int w, int h);
    void requestpaint(QtWidget* widget);                  // schedules a repaint (safe from any callback)
    void setpaintcolor(QtPainter* p, int r, int g, int b, int a = 255);   // pen and brush
    void setpenwidth(QtPainter* p, float px);
    void setfontsize(QtPainter* p, int px);
    void setantialias(QtPainter* p, bool on);
    void fillrect(QtPainter* p, int x, int y, int w, int h);
    void strokerect(QtPainter* p, int x, int y, int w, int h);
    void drawline(QtPainter* p, int x1, int y1, int x2, int y2);
    void fillellipse(QtPainter* p, int x, int y, int w, int h);
    void strokeellipse(QtPainter* p, int x, int y, int w, int h);
    void fillpolygon(QtPainter* p, const int* xy, int points);   // xy holds x0,y0,x1,y1,...
    void drawtext(QtPainter* p, int x, int y, const char* text); // (x, y) is the text baseline
};

struct Api {
    QtApi qt;                                         // evg.qt.createwindow / createbutton / ...
    void seterror(const char* message);               // lets optional modules report failures through error()

    // --- windows ---
    Window* createwindow(int width, int height, const char* title);   // headless (offscreen) on platforms without a backend
    // Use a window someone else created (a Qt widget's winId(), an SDL window's HWND, ...). evg draws into it and
    // never destroys it. `native` is an HWND on Windows.
    Window* attach(void* native, int width, int height);
    void destroywindow(Window* w);
    bool poll_close(Window*& which);
    bool poll_event(Event& e);                    // next queued event; Quit when a window is closed
    int  window_count() const;                    // open windows, in creation order
    Window* window_at(int i) const;
    Window* focus_window() const;                 // window that most recently produced input (null if none yet)
    void sleep(int ms);
    double time() const;                                              // seconds, monotonic

    // --- models ---
    Model* loadglb(const char* path);
    Model* loadglb_memory(const void* data, size_t size);
    void   freemodel(Model* m);

    // --- immediate-mode rendering (software rasterizer backend) ---
    bool begin(Window* w, float r, float g, float b);                 // clear; false if the window is minimized
    void camera(const Camera& c);                                     // sets view + projection from the window aspect
    void view_proj(const math::mat4& view, const math::mat4& proj);   // OpenGL clip conventions (see math::perspective)
    void light(const math::vec3<float>& direction, const math::vec3<float>& color, float ambient);
    void draw(const Model* m, const math::mat4& world = math::mat4());
    void end();                                                       // present to the window
    bool screenshot(Window* w, const char* path_bmp);                 // write the last frame as a 24-bit BMP

    // --- environment lighting ---
    // Load an OpenEXR (.exr) equirectangular environment. Needs the optional evg_exr module (EVG_WITH_EXR=ON);
    // without it this returns false and error() says so. Replaces the default directional light while active.
    bool import_lighting(const char* path_exr);
    bool import_lighting_raw(const float* rgb, int w, int h);   // linear RGB, no file needed
    void clear_lighting();
    bool has_lighting() const;

    // --- Vulkan probe (loader + instance + GPU pick; the Vulkan backend will build on this) ---
    bool init();
    // --- Vulkan (GPU) presentation, per window ---
    // Clears the window through the GPU and presents it. Needs a Vulkan driver with surface support; on failure
    // returns false and error() explains. The software path (begin/draw/end) is unaffected.
    bool vulkan_clear(Window* w, float r, float g, float b);
    const char* gpu() const;
    const char* error() const;
};

} // namespace evgx

extern evgx::Api evg;
