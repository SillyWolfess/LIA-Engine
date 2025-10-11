#include "simulation/physics.hpp"

void LIA::Physics::update(Object* object) {
    PhysicData& physics = object->_physics;
    CollisionData& collison = object->_collison;
    Speed& speed = physics._force;

    if (speed.x > 0.0f) {
        speed.x = speed.x - 0.01f;
        if (speed.x < 0.0) {
            speed.x = 0.0;
        }
    }
    // falling
    if (collison._grounded) {
        speed.y = 0;
        return;
    }
    speed.y += physics._mass;
    if (speed.y > 1) {
        speed.y = 1;
    }
}