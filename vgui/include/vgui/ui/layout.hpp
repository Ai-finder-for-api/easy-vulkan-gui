#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vgui/math/vec2.hpp"

namespace vgui {

/**
 * @brief Direction for layout
 */
enum class LayoutDirection {
    Horizontal,
    Vertical
};

/**
 * @brief Alignment for layout
 */
enum class LayoutAlignment {
    Start = 0,
    Center,
    End,
    Stretch
};

/**
 * @brief Wrap mode for layout
 */
enum class LayoutWrap {
    NoWrap,
    Wrap,
    WrapReverse
};

/**
 * @brief Distribution for layout
 */
enum class LayoutDistribution {
    Start,
    Center,
    End,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};

/**
 * @brief Scroll mode
 */
enum class ScrollMode {
    None,
    Horizontal,
    Vertical,
    VerticalAndHorizontal
};

/**
 * @brief Scroll bar visibility
 */
enum class ScrollBarVisibility {
    Auto,
    Always,
    Never
};

/**
 * @brief Layout class
 * 
 * Base class for all layout algorithms.
 */
class Layout {
public:
    /**
     * @brief Create a layout
     */
    Layout();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Layout();
    
    /**
     * @brief Get layout name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get layout type
     */
    virtual int getType() const = 0;
    
    /**
     * @brief Check if layout is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Set spacing between items
     */
    void setSpacing(float spacing);
    
    /**
     * @brief Get spacing
     */
    float getSpacing() const { return spacing_; }
    
    /**
     * @brief Set padding
     */
    void setPadding(float left, float top, float right, float bottom);
    
    /**
     * @brief Get padding
     */
    Vec4f getPadding() const { return padding_; }
    
    /**
     * @brief Set direction
     */
    virtual void setDirection(LayoutDirection direction);
    
    /**
     * @brief Get direction
     */
    LayoutDirection getDirection() const { return direction_; }
    
    /**
     * @brief Set alignment
     */
    void setAlignment(LayoutAlignment alignment);
    
    /**
     * @brief Get alignment
     */
    LayoutAlignment getAlignment() const { return alignment_; }
    
    /**
     * @brief Set secondary alignment (orthogonal to direction)
     */
    void setSecondaryAlignment(LayoutAlignment alignment);
    
    /**
     * @brief Get secondary alignment
     */
    LayoutAlignment getSecondaryAlignment() const { return secondaryAlignment_; }
    
    /**
     * @brief Set distribution
     */
    void setDistribution(LayoutDistribution distribution);
    
    /**
     * @brief Get distribution
     */
    LayoutDistribution getDistribution() const { return distribution_; }
    
    /**
     * @brief Set wrap mode
     */
    void setWrap(LayoutWrap wrap);
    
    /**
     * @brief Get wrap mode
     */
    LayoutWrap getWrap() const { return wrap_; }
    
    /**
     * @brief Set reversed
     */
    void setReversed(bool reversed);
    
    /**
     * @brief Check if reversed
     */
    bool isReversed() const { return reversed_; }
    
    /**
     * @brief Set grow priority
     */
    virtual void setGrowPriority(float priority);
    
    /**
     * @brief Get grow priority
     */
    float getGrowPriority() const { return growPriority_; }
    
    /**
     * @brief Set shrink priority
     */
    virtual void setShrinkPriority(float priority);
    
    /**
     * @brief Get shrink priority
     */
    float getShrinkPriority() const { return shrinkPriority_; }
    
    /**
     * @brief Check if layout is dirty
     */
    bool isDirty() const { return dirty_; }
    
    /**
     * @brief Mark layout as dirty
     */
    void setDirty(bool dirty);
    
    /**
     * @brief Force layout update
     */
    virtual void update();
    
    /**
     * @brief Calculate layout for children
     * 
     * @param parentWidth Parent width
     * @param parentHeight Parent height
     * @param children Children to layout
     * @param[out] positions Position for each child
     * @param[out] sizes Size for each child
     */
    virtual void layout(float parentWidth, float parentHeight,
                       const std::vector<std::shared_ptr<class Widget>>& children,
                       std::vector<Vec2f>& positions,
                       std::vector<Vec2f>& sizes);
    
    /**
     * @brief Get desired size for layout
     */
    virtual Vec2f getDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children);
    
    /**
     * @brief Reset layout
     */
    virtual void reset();
    
protected:
    bool valid_ = false;
    bool dirty_ = true;
    
    float spacing_ = 4.0f;
    Vec4f padding_ = Vec4f(4.0f);
    
    LayoutDirection direction_ = LayoutDirection::Vertical;
    LayoutAlignment alignment_ = LayoutAlignment::Start;
    LayoutAlignment secondaryAlignment_ = LayoutAlignment::Start;
    LayoutDistribution distribution_ = LayoutDistribution::Start;
    LayoutWrap wrap_ = LayoutWrap::NoWrap;
    bool reversed_ = false;
    
    float growPriority_ = 0.0f;
    float shrinkPriority_ = 0.0f;
    
    // Helper methods
    float distributeSpace(float availableSpace, float totalItemSize,
                         size_t itemCount, LayoutDistribution distribution);
    
    Vec2f alignItem(float itemSize, float availableSize,
                   LayoutAlignment alignment);
    
    virtual void doLayout(float parentWidth, float parentHeight,
                         const std::vector<std::shared_ptr<class Widget>>& children,
                         std::vector<Vec2f>& positions,
                         std::vector<Vec2f>& sizes) = 0;
    
    virtual Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children);
};

/**
 * @brief Horizontal layout
 */
class HorizontalLayout : public Layout {
public:
    HorizontalLayout();
    
    std::string getName() const override { return "Horizontal"; }
    int getType() const override { return 0; }
    
    void setDirection(LayoutDirection direction) override;
    void setWrap(LayoutWrap wrap) override;
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
};

/**
 * @brief Vertical layout
 */
class VerticalLayout : public Layout {
public:
    VerticalLayout();
    
    std::string getName() const override { return "Vertical"; }
    int getType() const override { return 1; }
    
    void setDirection(LayoutDirection direction) override;
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
};

/**
 * @brief Grid layout
 */
class GridLayout : public Layout {
public:
    /**
     * @brief Create grid layout
     */
    GridLayout();
    
    /**
     * @brief Set column count
     */
    void setColumnCount(size_t columns);
    
    /**
     * @brief Get column count
     */
    size_t getColumnCount() const { return columnCount_; }
    
    /**
     * @brief Set column widths
     */
    void setColumnWidths(const std::vector<float>& widths);
    
    /**
     * @brief Set column stretch factors
     */
    void setColumnStretch(const std::vector<float>& stretch);
    
    /**
     * @brief Set row heights
     */
    void setRowHeights(const std::vector<float>& heights);
    
    /**
     * @brief Set row stretch factors
     */
    void setRowStretch(const std::vector<float>& stretch);
    
    /**
     * @brief Set rows span
     */
    void setRowSpan(size_t row, size_t span);
    
    /**
     * @brief Set column span
     */
    void setColumnSpan(size_t col, size_t span);
    
    std::string getName() const override { return "Grid"; }
    int getType() const override { return 2; }
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
    
private:
    size_t columnCount_ = 2;
    size_t rowCount_ = 0;
    
    std::vector<float> columnWidths_;
    std::vector<float> columnStretch_;
    std::vector<float> rowHeights_;
    std::vector<float> rowStretch_;
    
    std::vector<size_t> rowSpan_;
    std::vector<size_t> columnSpan_;
    
    void calculateSpan(size_t childCount);
};

/**
 * @brief Flex layout (similar to CSS flexbox)
 */
class FlexLayout : public Layout {
public:
    /**
     * @brief Create flex layout
     */
    FlexLayout();
    
    /**
     * @brief Set flex growth
     */
    void setFlexGrow(float grow);
    
    /**
     * @brief Get flex grow
     */
    float getFlexGrow() const { return flexGrow_; }
    
    /**
     * @brief Set flex shrink
     */
    void setFlexShrink(float shrink);
    
    /**
     * @brief Get flex shrink
     */
    float getFlexShrink() const { return flexShrink_; }
    
    /**
     * @brief Set flex basis
     */
    void setFlexBasis(float basis);
    
    /**
     * @brief Get flex basis
     */
    float getFlexBasis() const { return flexBasis_; }
    
    /**
     * @brief Set flex direction
     */
    void setFlexDirection(LayoutDirection direction);
    
    /**
     * @brief Get flex direction
     */
    LayoutDirection getFlexDirection() const { return flexDirection_; }
    
    /**
     * @brief Set flex wrap
     */
    void setFlexWrap(LayoutWrap wrap);
    
    /**
     * @brief Get flex wrap
     */
    LayoutWrap getFlexWrap() const { return flexWrap_; }
    
    /**
     * @brief Set justify content
     */
    void setJustifyContent(LayoutDistribution justify);
    
    /**
     * @brief Get justify content
     */
    LayoutDistribution getJustifyContent() const { return justifyContent_; }
    
    /**
     * @brief Set align items
     */
    void setAlignItems(LayoutAlignment align);
    
    /**
     * @brief Get align items
     */
    LayoutAlignment getAlignItems() const { return alignItems_; }
    
    /**
     * @brief Set align content
     */
    void setAlignContent(LayoutAlignment align);
    
    /**
     * @brief Get align content
     */
    LayoutAlignment getAlignContent() const { return alignContent_; }
    
    /**
     * @brief Set align self (per-item override)
     */
    void setAlignSelf(LayoutAlignment align);
    
    /**
     * @brief Get align self
     */
    LayoutAlignment getAlignSelf() const { return alignSelf_; }
    
    std::string getName() const override { return "Flex"; }
    int getType() const override { return 3; }
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
    
private:
    float flexGrow_ = 0.0f;
    float flexShrink_ = 1.0f;
    float flexBasis_ = 0.0f;
    
    LayoutDirection flexDirection_ = LayoutDirection::Horizontal;
    LayoutWrap flexWrap_ = LayoutWrap::NoWrap;
    
    LayoutDistribution justifyContent_ = LayoutDistribution::Start;
    LayoutAlignment alignItems_ = LayoutAlignment::Stretch;
    LayoutAlignment alignContent_ = LayoutAlignment::Start;
    LayoutAlignment alignSelf_ = LayoutAlignment::Auto;
    
    // Flex-specific measurement
    struct FlexItem {
        std::shared_ptr<class Widget> widget;
        float desiredSize;
        float flexGrow;
        float flexShrink;
        float flexBasis;
        float minSize;
        float maxSize;
        LayoutAlignment alignSelf;
        
        float computedSize;
        float position;
        Vec2f finalSize;
    };
    
    std::vector<FlexItem> flexItems_;
    
    void measureFlexItems(const std::vector<std::shared_ptr<class Widget>>& children,
                         float containerWidth, float containerHeight);
    
    void layoutFlexItems(float containerWidth, float containerHeight);
};

/**
 * @brief Stack layout (children stacked on top of each other)
 */
class StackLayout : public Layout {
public:
    /**
     * @brief Create stack layout
     */
    StackLayout();
    
    /**
     * @brief Set stack alignment
     */
    void setStackAlignment(LayoutAlignment alignment);
    
    /**
     * @brief Get stack alignment
     */
    LayoutAlignment getStackAlignment() const { return stackAlignment_; }
    
    /**
     * @brief Set stretch mode
     */
    void setStretchMode(bool stretch);
    
    /**
     * @brief Check if stretching
     */
    bool getStretchMode() const { return stretch_; }
    
    std::string getName() const override { return "Stack"; }
    int getType() const override { return 4; }
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
    
private:
    LayoutAlignment stackAlignment_ = LayoutAlignment::Center;
    bool stretch_ = false;
};

/**
 * @brief Dock layout (similar to Qt dock widgets)
 */
class DockLayout : public Layout {
public:
    /**
     * @brief Create dock layout
     */
    DockLayout();
    
    /**
     * @brief Add dock widget
     */
    void addDock(std::shared_ptr<class Widget> widget,
                 DockPosition position = DockPosition::Right,
                 float size = 200.0f);
    
    /**
     * @brief Remove dock widget
     */
    void removeDock(std::shared_ptr<class Widget> widget);
    
    /**
     * @brief Set dock size
     */
    void setDockSize(std::shared_ptr<class Widget> widget, float size);
    
    /**
     * @brief Get dock size
     */
    float getDockSize(std::shared_ptr<class Widget> widget) const;
    
    /**
     * @brief Set dock position
     */
    void setDockPosition(std::shared_ptr<class Widget> widget, DockPosition position);
    
    /**
     * @brief Get dock position
     */
    DockPosition getDockPosition(std::shared_ptr<class Widget> widget) const;
    
    /**
     * @brief Clear all docks
     */
    void clearDocks();
    
    std::string getName() const override { return "Dock"; }
    int getType() const override { return 5; }
    
    void update() override;
    
protected:
    void doLayout(float parentWidth, float parentHeight,
                 const std::vector<std::shared_ptr<class Widget>>& children,
                 std::vector<Vec2f>& positions,
                 std::vector<Vec2f>& sizes) override;
    
    Vec2f doGetDesiredSize(const std::vector<std::shared_ptr<class Widget>>& children) override;
    
private:
    struct DockInfo {
        std::shared_ptr<class Widget> widget;
        DockPosition position;
        float size;
        float minSize;
        float maxSize;
    };
    
    std::vector<DockInfo> docks_;
    
    bool isDock(std::shared_ptr<class Widget> widget) const;
    DockInfo* findDock(std::shared_ptr<class Widget> widget);
    const DockInfo* findDock(std::shared_ptr<class Widget> widget) const;
};

} // namespace vgui
