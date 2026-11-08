#include "data/position.hpp"
#ifndef LIA_MATH
#define LIA_MATH
namespace LIA {
    class Object;
    struct CollidingObject {
        Position position;
        Scale scale;
    };
    class Math {
        public:
            static float inline PiOver180 = 0.0174532925f;
            static bool isColliding(Object*, Object*);
            static bool isColliding(CollidingObject*, CollidingObject*);
            static bool isColliding(CollidingObject*, Object*);
            static bool isColliding(CollidingObject*, Position, Scale);
            static bool isColliding(Object*, Position, Scale);
            static bool isColliding(Position, Scale, Position, Scale);

            static bool isInBounds(float source, float target, float bound);
            static int getRandomInt(int min, int max);

            static float cosOf(float);
            static float sinOf(float);

            static float sq(float);

            static float lengthOfSq(LIA::LIAV2);
            static float lengthOf(LIA::LIAV3);
            static float lengthOf(LIA::LIAV2);

            static void constrainPositionXY(LIA::Position& position, LIA::Position anker, float radius);
            static float toRadians(float);

            static float normilizer2D(LIA::LIAV3);
            static float normilizer(LIA::LIAV3);
            static float normilizer(LIA::LIAV2);
            static void normilize(LIA::LIAV3&);
            static void normilize(LIA::LIAV2&);

            static void mult(LIA::LIAV3& vec, float a);
            static void mult(LIA::LIAV2& vec, float a);
    };
}
#endif // !LIA_MATH