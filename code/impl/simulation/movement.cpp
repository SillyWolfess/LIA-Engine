#include "simulation/movement.hpp"
#include "math/Math.hpp"

bool LIA::Movement::isGrouned(int id, Object* object, ObjectManager& objManager, Position& newPosition) {
    int size = objManager.size();
    Position pos = object->_position;
    Scale scale = object->_scale;
    
    for (int i = 0; i < size; i++) {
        if (i == id) {
            continue;
        }
        Object* target = objManager.get(i);
        Position targetPos = target->_position;
        Scale targetScale = target->_scale;
        
        bool xLeft = Math::isInBounds(newPosition.x - scale.x, targetPos.x, targetScale.x);
        bool xRight = Math::isInBounds(newPosition.x + scale.x, targetPos.x, targetScale.x);
        bool yUp = Math::isInBounds(newPosition.y - scale.y, targetPos.y, targetScale.y);
        bool yDown = Math::isInBounds(newPosition.y + scale.y, targetPos.y, targetScale.y);

        if (!xLeft && !xRight && !yUp && !yDown) {
            continue;
        }
        if (yUp || yDown) {
            if (!xLeft && !xRight) {
                continue;
            }
            if (yDown) {
               return true;
            }      
        }
        if (xLeft || xRight) {
            if (!yUp && !yDown) {
                continue;
            }
            if (yDown) {
                return true;
            }
        }
    }
    return false;
}

bool LIA::Movement::isColliding(int id, Object* object, ObjectManager& objManager, Position& newPosition) {
    int size = objManager.size();
    const Position pos = object->_position;
    const Scale scale = object->_scale;
    
    for (int i = 0; i < size; i++) {
        if (i == id) {
            continue;
        }
        Object* target = objManager.get(i);
        if (Math::isColliding(target, object)) {
            return true;
        }
    }
    return false;
}

void LIA::Movement::update(int id, Object* object, ObjectManager& objManager) {
    MovementData& movementData = object->_movement;
    CollisionData& collisionData = object->_collison;

    const Speed speed = movementData._speed;
    Position newPosition;
    copy(newPosition, object->_position);
    newPosition.x += (speed.x * movementData._direction.x);
    newPosition.y += speed.y;
    newPosition.z += speed.z;
  
    collisionData._grounded = isGrouned(id, object, objManager, newPosition);
    Position movementCollider;
    copy(movementCollider, newPosition);
    movementCollider.y = object->_position.y - 0.1f;
    movementCollider.z = object->_position.z - 0.1f;
    const bool collision = isColliding(id, object, objManager, movementCollider);
    if (collision) {
        return;
    }
    object->_position.x = newPosition.x;
    
    if (!collisionData._grounded) {
        object->_position.y = newPosition.y;
        object->_position.z = newPosition.z;
    }
}
