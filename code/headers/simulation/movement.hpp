#ifndef LIA_MOVEMENT_HPP
#define LIA_MOVEMENT_HPP
#include "data/Object.hpp"
#include "manager/objectManager.hpp"

namespace LIA {
    class Movement {
        private:
            bool isInBounds(float, float, float);
            bool isGrouned(int, Object*, ObjectManager&, Position&);
            bool isColliding(int, Object*, ObjectManager&, Position&);
        public:
            void update(int, Object*, ObjectManager&);
    };
}
#endif