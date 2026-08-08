#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Node                                                   ║
// ║  Scene-graph node: transform + named children.                  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "transform.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace vgui {

class Node : public std::enable_shared_from_this<Node> {
public:
    std::string name;
    Transform   transform;
    bool        visible = true;

    static std::shared_ptr<Node> create(const std::string& name = "Node");

    // ── Hierarchy ─────────────────────────────────────────
    void                      add_child   (std::shared_ptr<Node> child);
    void                      remove_child(std::shared_ptr<Node> child);
    std::shared_ptr<Node>     find_child  (const std::string& name, bool recursive = true) const;

    Node*                     parent()   const { return m_parent; }
    const std::vector<std::shared_ptr<Node>>& children() const { return m_children; }

    // ── Traversal ─────────────────────────────────────────
    void traverse(std::function<void(Node&)> fn);
    void traverse(std::function<void(const Node&)> fn) const;

private:
    Node*                                    m_parent   = nullptr;
    std::vector<std::shared_ptr<Node>>       m_children;
};

} // namespace vgui
