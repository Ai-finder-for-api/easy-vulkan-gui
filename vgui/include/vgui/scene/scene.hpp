#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "vgui/scene/node.hpp"
#include "vgui/scene/entity.hpp"
#include "vgui/scene/light.hpp"
#include "vgui/scene/model.hpp"
#include "vgui/graphics/camera.hpp"

namespace vgui {

/**
 * @brief Scene class
 * 
 * Manages a collection of nodes, entities, and lights.
 * Provides scene-wide properties and updates.
 */
class Scene {
public:
    /**
     * @brief Create an empty scene
     */
    Scene();
    
    /**
     * @brief Destroy the scene
     */
    ~Scene();
    
    /**
     * @brief Check if scene is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get scene name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set scene name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get root node
     */
    Node& getRoot() { return rootNode_; }
    const Node& getRoot() const { return rootNode_; }
    
    /**
     * @brief Add a node to the scene
     */
    std::shared_ptr<Node> addNode(const std::string& name = "");
    
    /**
     * @brief Add a child node
     */
    std::shared_ptr<Node> addNode(std::shared_ptr<Node> parent,
                                   const std::string& name = "");
    
    /**
     * @brief Remove a node
     */
    void removeNode(std::shared_ptr<Node> node);
    
    /**
     * @brief Find node by name
     */
    std::shared_ptr<Node> findNode(const std::string& name) const;
    
    /**
     * @brief Find node by path
     */
    std::shared_ptr<Node> findNodeByPath(const std::string& path) const;
    
    /**
     * @brief Get all nodes
     */
    const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes_; }
    
    /**
     * @brief Get node count
     */
    size_t getNodeCount() const { return nodes_.size(); }
    
    /**
     * @brief Get entities
     */
    const std::vector<std::shared_ptr<Entity>>& getEntities() const { return entities_; }
    
    /**
     * @brief Add entity
     */
    std::shared_ptr<Entity> addEntity(const std::string& name = "");
    
    /**
     * @brief Remove entity
     */
    void removeEntity(std::shared_ptr<Entity> entity);
    
    /**
     * @brief Find entity
     */
    std::shared_ptr<Entity> findEntity(const std::string& name) const;
    
    /**
     * @brief Get entity count
     */
    size_t getEntityCount() const { return entities_.size(); }
    
    /**
     * @brief Get lights
     */
    const std::vector<std::shared_ptr<Light>>& getLights() const { return lights_; }
    
    /**
     * @brief Add light
     */
    std::shared_ptr<Light> addLight(const std::string& name = "");
    
    /**
     * @brief Remove light
     */
    void removeLight(std::shared_ptr<Light> light);
    
    /**
     * @brief Get main camera
     */
    Camera& getMainCamera() { return mainCamera_; }
    const Camera& getMainCamera() const { return mainCamera_; }
    
    /**
     * @brief Set main camera
     */
    void setMainCamera(const Camera& camera) { mainCamera_ = camera; }
    
    /**
     * @brief Update scene
     */
    void update(float deltaTime);
    
    /**
     * @brief Update node transform hierarchy
     */
    void updateTransforms();
    
    /**
     * @brief Get scene bounding box
     */
    BoundingBox getBounds() const;
    
    /**
     * @brief Clear scene
     */
    void clear();
    
    /**
     * @brief Serialize scene to file
     */
    bool saveToFile(const std::string& path);
    
    /**
     * @brief Load scene from file
     */
    bool loadFromFile(const std::string& path);
    
    /**
     * @brief Create default scene with primitives
     */
    static std::shared_ptr<Scene> createDefaultScene();
    
    /**
     * @brief Create empty scene
     */
    static std::shared_ptr<Scene> create();
    
private:
    bool valid_ = false;
    std::string name_ = "Unnamed Scene";
    
    Node rootNode_;
    std::vector<std::shared_ptr<Node>> nodes_;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodeMap_;
    
    std::vector<std::shared_ptr<Entity>> entities_;
    std::unordered_map<std::string, std::shared_ptr<Entity>> entityMap_;
    
    std::vector<std::shared_ptr<Light>> lights_;
    
    Camera mainCamera_;
    
    bool needsTransformUpdate_ = true;
    
    void removeNodeFromMap(std::shared_ptr<Node> node);
    void collectNodes(Node& node, std::vector<std::shared_ptr<Node>>& out);
};

/**
 * @brief Scene manager
 * 
 * Manages multiple scenes and scene transitions.
 */
class SceneManager {
public:
    /**
     * @brief Get instance
     */
    static SceneManager& getInstance();
    
    /**
     * @brief Create scene
     */
    std::shared_ptr<Scene> createScene(const std::string& name = "");
    
    /**
     * @brief Get scene
     */
    std::shared_ptr<Scene> getScene(const std::string& name);
    
    /**
     * @brief Get active scene
     */
    std::shared_ptr<Scene> getActiveScene() const { return activeScene_; }
    
    /**
     * @brief Set active scene
     */
    void setActiveScene(std::shared_ptr<Scene> scene);
    
    /**
     * @brief Check if scene exists
     */
    bool hasScene(const std::string& name) const;
    
    /**
     * @brief Remove scene
     */
    void removeScene(const std::string& name);
    
    /**
     * @brief Get scene count
     */
    size_t getSceneCount() const { return scenes_.size(); }
    
    /**
     * @brief Get all scene names
     */
    std::vector<std::string> getSceneNames() const;
    
    /**
     * @brief Load scene from file
     */
    std::shared_ptr<Scene> loadScene(const std::string& path);
    
    /**
     * @brief Save scene to file
     */
    bool saveScene(const std::string& path, std::shared_ptr<Scene> scene);
    
private:
    SceneManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes_;
    std::shared_ptr<Scene> activeScene_;
};

} // namespace vgui
