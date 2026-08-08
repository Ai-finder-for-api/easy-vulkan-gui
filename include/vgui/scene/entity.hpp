#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Entity                                                 ║
// ║  Lightweight entity – an ID + component bag.                    ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto e  = scene.create_entity("Player");                     ║
// ║    auto& t = e.add<Transform>();                                ║
// ║    auto& m = e.add<MeshRenderer>(my_mesh, my_material);        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

namespace vgui {

using EntityID = uint32_t;
inline constexpr EntityID NULL_ENTITY = 0;

class Entity {
public:
    EntityID    id   = NULL_ENTITY;
    std::string name;

    template<typename T, typename... Args>
    T& add(Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        m_components[std::type_index(typeid(T))] = comp;
        return *comp;
    }

    template<typename T>
    T* get() const {
        auto it = m_components.find(std::type_index(typeid(T)));
        return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    bool has() const { return get<T>() != nullptr; }

    template<typename T>
    void remove() { m_components.erase(std::type_index(typeid(T))); }

    bool valid() const { return id != NULL_ENTITY; }

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;
};

} // namespace vgui
