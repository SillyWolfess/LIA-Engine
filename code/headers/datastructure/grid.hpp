#ifndef LIA_GRID_HPP
#define LIA_GRID_HPP

#include "./../manager/objectManager.hpp"
#include "./../data/position.hpp"
#include <map>

namespace LIA {
    class Grid {
        private:
            Position _position;
            Scale _scale;
            int _dX, _dY;
            std::map<int, std::string> _objectDef;
        public:
            void setPosition(Position&);
            void setScale(Scale&);
            void setDimension(int, int);
            bool load(ObjectManager&);
            bool init(std::string);
    };
}
#endif