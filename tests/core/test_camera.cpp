// Camera controllers: free flight (fly) and the orbit camera (update, frame, camera).
#include "../common/check.hpp"

int main() {
    using namespace evgx;
    using evgtest::near;

    // forward at yaw 90 degrees points along +x; yaw 0 looks down -z
    Camera c;
    c.yaw = 1.5707963f;
    CHECK(near(c.forward().x, 1.f, 1e-3f) && near(c.forward().z, 0.f, 1e-3f));
    c.yaw = 0.f;
    CHECK(near(c.forward().z, -1.f, 1e-3f));

    // flight: W for half a second at speed 3 moves 1.5 units forward
    evgx::Window* w = evg.createwindow(64, 48, "camera");
    CHECK(w != nullptr);
    if (!w) return evgtest::finish("core/camera");
    w->open();
    w->on_key('W', true);
    w->dt = 0.5f;
    Camera fly;                                      // starts at (0, 0, 5)
    fly.fly(*w, 3.f);
    CHECK(near(fly.pos.z, 3.5f, 1e-3f) && near(fly.pos.x, 0.f, 1e-3f));
    w->on_key('W', false);

    // strafe right with shift held: x3 speed
    w->on_key('D', true);
    w->on_key(key::Shift, true);
    w->dt = 1.0f;
    Camera strafe;
    strafe.fly(*w, 2.f);
    CHECK(near(strafe.pos.x, 6.f, 1e-3f));          // 2 * 3 (shift) * 1 s, to the right of yaw 0 (+x)
    w->on_key('D', false);
    w->on_key(key::Shift, false);

    // right-drag rotates the view: yaw += dx * 0.0035, pitch -= dy * 0.0035
    Camera look;
    w->on_button(1, true);
    w->dx = 100.f;
    w->dy = 50.f;
    w->dt = 0.f;
    look.fly(*w, 0.f);
    CHECK(near(look.yaw, 0.35f, 1e-4f) && near(look.pitch, -0.175f, 1e-4f));
    w->on_button(1, false);

    // orbit: the wheel zooms in (distance shrinks by 0.9 per notch, up to 20 notches)
    Orbit o;
    o.dist = 10.f;
    w->wheel = 2.f;
    w->dx = 0.f;
    w->dy = 0.f;
    o.update(*w);
    CHECK(near(o.dist, 8.1f, 1e-3f));

    // orbit: left-drag changes yaw by dx * 0.01
    o.yaw = 0.f;
    w->wheel = 0.f;
    w->on_button(0, true);
    w->dx = 100.f;
    o.update(*w);
    CHECK(near(o.yaw, 1.0f, 1e-4f));
    w->on_button(0, false);

    // frame() aims at the model's centre and backs off by the bounding radius
    Model fake;
    fake.bmin = math::vec3<float>(-1.f, -1.f, -1.f);
    fake.bmax = math::vec3<float>(1.f, 1.f, 1.f);
    Orbit framed;
    framed.frame(&fake);
    CHECK(near(framed.target.x, 0.f) && near(framed.target.y, 0.f) && near(framed.target.z, 0.f));
    CHECK(framed.dist > 3.f && framed.dist < 10.f);

    // camera() places the eye on the orbit sphere, looking back at the target
    Camera eye = o.camera();
    math::vec3<float> to = eye.pos - o.target;
    CHECK(near(math::length(to), o.dist, 1e-3f));

    evg.destroywindow(w);
    return evgtest::finish("core/camera");
}
