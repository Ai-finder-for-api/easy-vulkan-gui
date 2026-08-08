#pragma once

#include <cmath>
#include <string>
#include "vgui/math/vec3.hpp"
#include "vgui/math/vec4.hpp"
#include "vgui/math/mat4.hpp"
#include "vgui/math/quat.hpp"

namespace vgui {

/**
 * @brief Projection type
 */
enum class ProjectionType {
    Perspective,
    Orthographic,
    Custom  // User provides their own projection matrix
};

/**
 * @brief Camera class
 * 
 * Manages view and projection matrices for rendering.
 * Supports perspective and orthographic projections.
 */
class Camera {
public:
    /**
     * @brief Create a default camera
     */
    Camera();
    
    /**
     * @brief Create with position and target
     */
    Camera(const Vec3f& position, const Vec3f& target, 
          const Vec3f& up = Vec3f::UNIT_Y);
    
    /**
     * @brief Set position
     */
    void setPosition(const Vec3f& position);
    
    /**
     * @brief Get position
     */
    Vec3f getPosition() const { return position_; }
    
    /**
     * @brief Set target (look-at point)
     */
    void setTarget(const Vec3f& target);
    
    /**
     * @brief Get target
     */
    Vec3f getTarget() const { return target_; }
    
    /**
     * @brief Set up vector
     */
    void setUp(const Vec3f& up);
    
    /**
     * @brief Get up vector
     */
    Vec3f getUp() const { return up_; }
    
    /**
     * @brief Set right vector (computed from target and up)
     */
    Vec3f getRight() const { 
        return getForward().cross(getUp()).normalized(); 
    }
    
    /**
     * @brief Set field of view (perspective)
     */
    void setFOVY(float fovY);
    
    /**
     * @brief Get FOV Y
     */
    float getFOVY() const { return fovY_; }
    
    /**
     * @brief Set aspect ratio
     */
    void setAspect(float aspect);
    
    /**
     * @brief Get aspect ratio
     */
    float getAspect() const { return aspect_; }
    
    /**
     * @brief Set near plane
     */
    void setNear(float near);
    
    /**
     * @brief Get near plane
     */
    float getNear() const { return near_; }
    
    /**
     * @brief Set far plane
     */
    void setFar(float far);
    
    /**
     * @brief Get far plane
     */
    float getFar() const { return far_; }
    
    /**
     * @brief Set orthographic size
     */
    void setOrthoSize(float size);
    
    /**
     * @brief Get orthographic size
     */
    float getOrthoSize() const { return orthoSize_; }
    
    /**
     * @brief Set projection type
     */
    void setProjectionType(ProjectionType type);
    
    /**
     * @brief Get projection type
     */
    ProjectionType getProjectionType() const { return projType_; }
    
    /**
     * @brief Look at a target
     */
    void lookAt(const Vec3f& target);
    
    /**
     * @brief Look at with position
     */
    void lookAt(const Vec3f& position, const Vec3f& target, 
                const Vec3f& up = Vec3f::UNIT_Y);
    
    /**
     * @brief Get view matrix
     */
    Mat4f getViewMatrix() const;
    
    /**
     * @brief Get projection matrix
     */
    Mat4f getProjectionMatrix() const;
    
    /**
     * @brief Get view-projection matrix
     */
    Mat4f getViewProjectionMatrix() const;
    
    /**
     * @brief Set custom projection matrix
     */
    void setCustomProjection(const Mat4f& matrix);
    
    /**
     * @brief Get custom projection
     */
    Mat4f getCustomProjection() const { return customProjection_; }
    
    /**
     * @brief Get forward direction
     */
    Vec3f getForward() const {
        return (target_ - position_).normalized();
    }
    
    /**
     * @brief Get backward direction
     */
    Vec3f getBackward() const { return -getForward(); }
    
    /**
     * @brief Get left direction
     */
    Vec3f getLeft() const {
        return -getRight();
    }
    
    /**
     * @brief Move camera
     */
    void move(const Vec3f& delta);
    
    /**
     * @brief Move in local space
     */
    void moveLocal(const Vec3f& localDelta);
    
    /**
     * @brief Rotate camera
     */
    void rotate(const Vec3f& axis, float angle);
    
    /**
     * @brief Rotate around target
     */
    void rotateAroundTarget(const Vec3f& axis, float angle);
    
    /**
     * @brief Zoom (adjust FOV)
     */
    void zoom(float factor);
    
    /**
     * @brief Orbit around target
     */
    void orbit(float yawDelta, float pitchDelta);
    
    /**
     * @brief Reset to default orientation
     */
    void reset();
    
    /**
     * @brief Get camera matrix (for shader uniforms)
     */
    Mat4f getCameraMatrix() const { return getViewMatrix(); }
    
    /**
     * @brief Get inverse view matrix
     */
    Mat4f getInverseViewMatrix() const;
    
    /**
     * @brief Convert screen coordinates to world ray
     */
    void screenToWorldRay(float screenX, float screenY, 
                          uint32_t screenWidth, uint32_t screenHeight,
                          Vec3f& origin, Vec3f& direction) const;
    
    /**
     * @brief Project world point to screen
     */
    Vec3f worldToScreen(const Vec3f& worldPos,
                       uint32_t screenWidth, uint32_t screenHeight) const;
    
    /**
     * @brief Unproject screen point to world
     */
    Vec3f screenToWorld(float screenX, float screenY, float depth,
                       uint32_t screenWidth, uint32_t screenHeight) const;
    
    /**
     * @brief Check if point is visible
     */
    bool isVisible(const Vec3f& worldPos) const;
    
    /**
     * @brief Get frustum planes
     */
    struct FrustumPlanes {
        Vec4f left;
        Vec4f right;
        Vec4f top;
        Vec4f bottom;
        Vec4f near;
        Vec4f far;
    };
    
    FrustumPlanes getFrustumPlanes() const;
    
    /**
     * @brief Check if sphere is visible
     */
    bool sphereVisible(const Vec3f& center, float radius) const;
    
    /**
     * @brief Check if box is visible
     */
    bool boxVisible(const Vec3f& min, const Vec3f& max) const;
    
    /**
     * @brief Get camera name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Check if camera is active
     */
    bool isActive() const { return active_; }
    
    /**
     * @brief Set active
     */
    void setActive(bool active) { active_ = active; }
    
    // Common camera presets
    static Camera createPerspective(float fovY = 45.0f * 3.14159f / 180.0f,
                                    float aspect = 16.0f / 9.0f,
                                    float near = 0.1f, float far = 1000.0f);
    
    static Camera createOrthographic(float size = 10.0f,
                                     float aspect = 16.0f / 9.0f,
                                     float near = -100.0f, float far = 100.0f);
    
    static Camera createUIOrthographic(uint32_t width, uint32_t height);
    
private:
    Vec3f position_;
    Vec3f target_;
    Vec3f up_;
    
    float fovY_ = 45.0f * 3.14159f / 180.0f;  // Radians
    float aspect_ = 16.0f / 9.0f;
    float near_ = 0.1f;
    float far_ = 1000.0f;
    float orthoSize_ = 10.0f;
    
    ProjectionType projType_ = ProjectionType::Perspective;
    Mat4f customProjection_;
    
    bool active_ = true;
    std::string name_ = "Camera";
    
    // Cached matrices
    mutable Mat4f viewMatrix_;
    mutable bool viewMatrixDirty_ = true;
    
    // Recompute view matrix
    void updateViewMatrix() const;
    
    // Check if view matrix needs update
    bool isViewDirty() const { return viewMatrixDirty_; }
};

/**
 * @brief Camera controller for user input
 */
class CameraController {
public:
    /**
     * @brief Create controller
     */
    CameraController(Camera& camera);
    
    /**
     * @brief Update controller
     */
    void update(float deltaTime);
    
    /**
     * @brief Set movement speed
     */
    void setMovementSpeed(float speed);
    
    /**
     * @brief Set rotation speed
     */
    void setRotationSpeed(float speed);
    
    /**
     * @brief Set zoom speed
     */
    void setZoomSpeed(float speed);
    
    /**
     * @brief Handle keyboard input
     */
    void handleKey(uint32_t key, uint32_t action);
    
    /**
     * @brief Handle mouse movement
     */
    void handleMouseMove(double x, double y, bool capture = true);
    
    /**
     * @brief Handle scroll
     */
    void handleScroll(double xoffset, double yoffset);
    
    /**
     * @brief Set capture mouse
     */
    void setCaptureMouse(bool capture);
    
    /**
     * @brief Check if mouse is captured
     */
    bool isMouseCaptured() const { return captureMouse_; }
    
    /**
     * @brief Reset to default values
     */
    void reset();
    
private:
    Camera& camera_;
    float movementSpeed_ = 10.0f;
    float rotationSpeed_ = 0.5f;
    float zoomSpeed_ = 1.0f;
    bool captureMouse_ = false;
    double lastMouseX_ = 0.0;
    double lastMouseY_ = 0.0;
    
    // Movement state
    bool moveForward_ = false;
    bool moveBackward_ = false;
    bool moveLeft_ = false;
    bool moveRight_ = false;
    bool moveUp_ = false;
    bool moveDown_ = false;
    
    void updateMovement(float deltaTime);
};

} // namespace vgui
