#include <vgui/ui/textbox.hpp>
#include <vgui/input/keyboard.hpp>
#include <vgui/input/mouse.hpp>

namespace vgui {

std::shared_ptr<Textbox> Textbox::create(const std::string& hint) {
    auto t = std::shared_ptr<Textbox>(new Textbox);
    t->placeholder = hint; return t;
}

void Textbox::focus() { focused = true; }
void Textbox::blur()  { focused = false; }

void Textbox::update(float dt) {
    if (!visible || !enabled) return;

    // Click to focus
    if (Mouse::get().pressed(MouseButton::Left)) {
        focused = contains(Mouse::get().position());
    }

    if (!focused) return;

    m_blink_t += dt;
    if (m_blink_t > 1.0f) m_blink_t = 0.0f;

    auto& kb = Keyboard::get();

    // Backspace
    if (kb.pressed(Key::Backspace) && m_cursor > 0) {
        text.erase(--m_cursor, 1);
        if (on_change) on_change(text);
    }
    // Delete
    if (kb.pressed(Key::Delete) && m_cursor < text.size()) {
        text.erase(m_cursor, 1);
        if (on_change) on_change(text);
    }
    // Navigate
    if (kb.pressed(Key::Left)  && m_cursor > 0)           --m_cursor;
    if (kb.pressed(Key::Right) && m_cursor < text.size())  ++m_cursor;
    if (kb.pressed(Key::Home)) m_cursor = 0;
    if (kb.pressed(Key::End))  m_cursor = text.size();
    // Submit
    if (kb.pressed(Key::Enter) && on_submit) on_submit(text);
}

void Textbox::draw() {
    if (!visible) return;
    // Canvas draw calls omitted – placeholder
    for (auto& c : m_children) c->draw();
}

} // namespace vgui
