// Qt 6 canvas (optional: configure with -DEVG_WITH_QT=ON). evg presents through Vulkan into a QWidget and receives its input.
// Header-only; no moc needed (no signals or slots).
//
//   evgx::qt::Canvas canvas;          // top-level or child widget
//   canvas.show();                    // evg attaches on first show; keys and mouse go to canvas.window()
//
// Subclass Canvas and override frame(Window*) to draw with evg; read input from the Window's keys/buttons/mouse fields.
#pragma once
#include <QWidget>
#include <QTimer>
#include <QShowEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <evg/evg.hpp>

namespace evgx { namespace qt {

// Qt key -> evg key code (letters and digits are uppercase ASCII, the rest are evgx::key::*). 0 when unknown.
inline int evg_key(int k) {
    if ((k >= Qt::Key_A && k <= Qt::Key_Z) || (k >= Qt::Key_0 && k <= Qt::Key_9)) return k;
    switch (k) {
        case Qt::Key_Return: case Qt::Key_Enter: return key::Enter;
        case Qt::Key_Escape: return key::Esc;
        case Qt::Key_Tab: return key::Tab;
        case Qt::Key_Backspace: return 8;
        case Qt::Key_Space: return key::Space;
        case Qt::Key_Left: return key::Left;
        case Qt::Key_Up: return key::Up;
        case Qt::Key_Right: return key::Right;
        case Qt::Key_Down: return key::Down;
        case Qt::Key_Shift: return key::Shift;
        case Qt::Key_Control: return key::Ctrl;
        case Qt::Key_Alt: return 18;
    }
    return 0;
}

// Qt mouse button -> evg button index (0 left, 1 right, 2 middle), -1 when unknown.
inline int evg_button(Qt::MouseButton b) {
    if (b == Qt::LeftButton) return 0;
    if (b == Qt::RightButton) return 1;
    if (b == Qt::MiddleButton) return 2;
    return -1;
}

class Canvas : public QWidget {
public:
    explicit Canvas(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_NativeWindow, true);       // evg needs a real native window
        setAttribute(Qt::WA_PaintOnScreen, true);      // Qt must not paint over what evg presents
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setMouseTracking(true);                        // move events without a pressed button
        setFocusPolicy(Qt::StrongFocus);               // receive key events after a click
        timer_.setInterval(0);                         // present again as soon as the event loop is idle
        QObject::connect(&timer_, &QTimer::timeout, this, &Canvas::tick);
    }
    ~Canvas() override {
        timer_.stop();
        if (win_) evg.destroywindow(win_);             // Qt owns the HWND; evg only drops its state
    }

    // Called once per presented frame. Default: clear to the clear color through the GPU.
    virtual void frame(Window* w) { evg.vulkan_clear(w, r_, g_, b_); }

    void setClearColor(float r, float g, float b) { r_ = r; g_ = g; b_ = b; }
    Window* window() const { return win_; }
    double fps() const { return fps_; }               // presentation rate over the last second

protected:
    void showEvent(QShowEvent* e) override {
        QWidget::showEvent(e);
        if (!win_) {
            win_ = evg.attach((void*)winId(), width(), height());
            if (win_) { fpsT0_ = evg.time(); timer_.start(); }
        }
    }
    void resizeEvent(QResizeEvent* e) override {
        QWidget::resizeEvent(e);
        if (win_) { win_->width = width(); win_->height = height(); }
    }

    // ---- input: forwarded to the evg Window (its keys/buttons/mouse/wheel fields and event queue) ----
    void keyPressEvent(QKeyEvent* e) override {
        if (win_) win_->on_key(evg_key(e->key()), true, e->isAutoRepeat());
        else QWidget::keyPressEvent(e);
    }
    void keyReleaseEvent(QKeyEvent* e) override {
        if (win_ && !e->isAutoRepeat()) win_->on_key(evg_key(e->key()), false);
        else QWidget::keyReleaseEvent(e);
    }
    void mousePressEvent(QMouseEvent* e) override {
        setFocus();
        if (!win_) { QWidget::mousePressEvent(e); return; }
        win_->on_mouse_move(static_cast<int>(e->position().x()), static_cast<int>(e->position().y()));
        int b = evg_button(e->button());
        if (b >= 0) win_->on_button(b, true);
    }
    void mouseReleaseEvent(QMouseEvent* e) override {
        if (!win_) { QWidget::mouseReleaseEvent(e); return; }
        int b = evg_button(e->button());
        if (b >= 0) win_->on_button(b, false);
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        if (!win_) { QWidget::mouseMoveEvent(e); return; }
        win_->on_mouse_move(static_cast<int>(e->position().x()), static_cast<int>(e->position().y()));
    }
    void wheelEvent(QWheelEvent* e) override {
        if (!win_) { QWidget::wheelEvent(e); return; }
        win_->on_wheel(e->angleDelta().y() / 120.0f);  // one notch = 120 units
    }

private:
    void tick() {
        if (!win_ || win_->closed || width() <= 0 || height() <= 0) return;
        win_->width = width(); win_->height = height();
        frame(win_);
        frames_++;
        double now = evg.time();
        if (now - fpsT0_ >= 1.0) { fps_ = frames_ / (now - fpsT0_); frames_ = 0; fpsT0_ = now; }
    }

    Window* win_ = nullptr;
    QTimer timer_;
    float r_ = 0.1f, g_ = 0.12f, b_ = 0.18f;
    int frames_ = 0;
    double fpsT0_ = 0, fps_ = 0;
};

}}  // namespace evgx::qt
