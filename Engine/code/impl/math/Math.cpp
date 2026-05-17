#include "math/Math.hpp"
#include "data/Object.hpp"
#include <random>

bool LIA::Math::isColliding(Object* target, Object* source) {
    // TODO needs to do reverse check, single does not work if target is smaller than source
    return isColliding(target, source->_position, source->_scale) || isColliding(source, target->_position, target->_scale);
}

float LIA::Math::toRadians(float euql) {
    return euql * PiOver180;
}

bool LIA::Math::isColliding(Object* target, Position newPosition, Scale scale) {
    const Position targetPos = target->_position;
    const Scale targetScale = target->_scale;
        
    bool xLeft = isInBounds(newPosition.x - scale.x, targetPos.x, targetScale.x);
    bool xRight = isInBounds(newPosition.x + scale.x, targetPos.x, targetScale.x);
    bool yUp = isInBounds(newPosition.y - scale.y, targetPos.y, targetScale.y);
    bool yDown = isInBounds(newPosition.y + scale.y, targetPos.y, targetScale.y);

    if (!xLeft && !xRight && !yUp && !yDown) {
        return false;
    }
    if (yUp || yDown) {
        if (!xLeft && !xRight) {
              return false;
        }     
        return true;
    }
    if (xLeft || xRight) {
        if (!yUp && !yDown) {
               return false;
        }
        return true;
    }
    return false;
}

bool LIA::Math::isInBounds(float source, float target, float bound) {
    return source >= target - bound && source <= target + bound;
}

int LIA::Math::getRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

#include <math.h>
float LIA::Math::cosOf(float angle) {
    return std::cos(angle);
}

float LIA::Math::sinOf(float angle) {
    return std::sin(angle);
}

float LIA::Math::lengthOfSq(LIAV2 vector) {
    return (vector.x * vector.x) + (vector.y * vector.y);
}

float LIA::Math::sq(float x) {
    return x * x;
}

void LIA::Math::constrainPositionXY(Position& position, Position anker, float radius) {
    if (radius <= 0) {
        return;
    }
    LIAV2 vec;
    vec.x = position.x - anker.x;
    vec.y = position.y - anker.y;

    float lengthSq = lengthOfSq(vec);

    if (lengthSq <= radius * radius) {
        return;
    }

    float length = std::sqrt(lengthSq);
    float scale = radius / length;

    position.x = anker.x + (scale * vec.x);
    position.y = anker.y + (scale * vec.y);
}
