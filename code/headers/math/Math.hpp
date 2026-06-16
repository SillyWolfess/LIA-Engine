#include "data/position.hpp"

namespace LIA {
    class Object;
    class Math {
        public:
            static float inline PiOver180 = 0.0174532925f;
            static bool isColliding(Object*, Object*);
            static bool isColliding(Object*, Position, Scale);
            static bool isInBounds(float source, float target, float bound);
            static int getRandomInt(int min, int max);
            static float cosOf(float);
            static float sinOf(float);
            static float sq(float);
            static float lengthOfSq(LIA::LIAV2);
            static void constrainPositionXY(LIA::Position& position, LIA::Position anker, float radius);
            static float toRadians(float);
    };
}