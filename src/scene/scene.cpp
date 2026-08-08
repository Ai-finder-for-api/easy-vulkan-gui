#include <vgui/scene/scene.hpp>
#include <algorithm>

namespace vgui {

std::shared_ptr<Scene> Scene::create(const std::string& n) {
    auto s   = std::shared_ptr<Scene>(new Scene);
    s->name  = n;
    s->m_root= Node::create("Root");
    return s;
}

Entity& Scene::create_entity(const std::string& n) {
    Entity e; e.id = m_next_id++; e.name = n;
    m_entities.push_back(std::move(e));
    return m_entities.back();
}

void Scene::destroy_entity(EntityID id) {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [id](const Entity& e){ return e.id == id; }),
        m_entities.end());
}

Entity* Scene::find_entity(const std::string& n) const {
    for (auto& e : const_cast<std::vector<Entity>&>(m_entities))
        if (e.name == n) return &e;
    return nullptr;
}

Entity* Scene::get_entity(EntityID id) const {
    for (auto& e : const_cast<std::vector<Entity>&>(m_entities))
        if (e.id == id) return &e;
    return nullptr;
}

void Scene::update(float dt) {
    m_root->traverse([dt](Node&) { /* system updates would go here */ });
}

void Scene::render() { /* delegate to renderer */ }

} // namespace vgui
