#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Clipboard                                              ║
// ║  Read and write the system clipboard. UTF-8 text only.          ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>

namespace vgui {

class Clipboard {
public:
    static Clipboard& get();

    std::string get_text()                    const;
    void        set_text(const std::string& s);
    bool        has_text()                    const;
    void        clear();

private:
    Clipboard() = default;
};

} // namespace vgui
