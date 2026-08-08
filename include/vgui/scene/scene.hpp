#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Scene                                                  ║
// ║  Container for entities, lights, and the scene graph root.      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "entity.hpp"
#include "node.hpp"
#include "camera.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace vgui {

class Scene {
public:
    std::string name;

    static std::shared_ptr<Scene> create(const std::string& name = "Scene");

    // ── Entity management ─────────────────────────────────
    Entity&  create_entity(const std::string& name = "Entity");
    void     destroy_entity(EntityID id);
    Entity*  find_entity   (const std::string& name) const;
    Entity*  get_entity    (EntityID id)              const;

    const std::vector<Entity>& entities() const { return m_entities; }

    // ── Iteration helpers ─────────────────────────────────
    template<typename T>
    void for_each(std::function<void(Entity&, T&)> fn) {
        for (auto& e : m_entities)
            if (auto* c = e.get<T>()) fn(e, *c);
    }

    // ── Scene graph ───────────────────────────────────────
    Node& root() { return *m_root; }

    // ── Active camera ─────────────────────────────────────
    Camera* active_camera = nullptr;

    // ── Update / render ───────────────────────────────────
    void update(float dt);
    void render();

private:
    Scene() = default;
    std::vector<Entity>        m_entities;
    std::shared_ptr<Node>      m_root;
    EntityID                   m_next_id = 1;
};

} // namespace vgui
