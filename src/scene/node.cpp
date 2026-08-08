#include <vgui/scene/node.hpp>
#include <algorithm>

namespace vgui {

std::shared_ptr<Node> Node::create(const std::string& n) {
    auto p = std::shared_ptr<Node>(new Node);
    p->name = n;
    return p;
}

void Node::add_child(std::shared_ptr<Node> child) {
    child->m_parent = this;
    child->transform.set_parent(&transform);
    m_children.push_back(std::move(child));
}

void Node::remove_child(std::shared_ptr<Node> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        (*it)->transform.set_parent(nullptr);
        m_children.erase(it);
    }
}

std::shared_ptr<Node> Node::find_child(const std::string& n, bool recursive) const {
    for (auto& c : m_children) {
        if (c->name == n) return c;
        if (recursive) if (auto r = c->find_child(n, true)) return r;
    }
    return nullptr;
}

void Node::traverse(std::function<void(Node&)> fn) {
    fn(*this);
    for (auto& c : m_children) c->traverse(fn);
}

void Node::traverse(std::function<void(const Node&)> fn) const {
    fn(*this);
    for (auto& c : m_children) c->traverse(fn);
}

} // namespace vgui
