#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Layout                                                 ║
// ║  Automatic child arrangement (stack, grid, flex-row).           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include <memory>

namespace vgui {

enum class LayoutType : uint8_t {
    None,       ///< Manual placement
    VStack,     ///< Vertical stack top-to-bottom
    HStack,     ///< Horizontal stack left-to-right
    Grid,       ///< Fixed-column grid
    Flex        ///< Flex-row (wraps onto next line)
};

class Layout : public Widget {
public:
    LayoutType type    = LayoutType::VStack;
    float      gap     = 4.0f;      ///< Space between children
    float      pad_x   = 0.0f;      ///< Inner horizontal padding
    float      pad_y   = 0.0f;      ///< Inner vertical padding
    int        columns = 2;          ///< Grid only: number of columns

    static std::shared_ptr<Layout> create(LayoutType type = LayoutType::VStack);

    /// Recompute child positions. Called automatically before draw.
    void arrange();

    void update(float dt) override;
    void draw()           override;

private:
    Layout() = default;
};

} // namespace vgui
