#ifndef BASE_GAME_HPP
#define BASE_GAME_HPP
#include "component/Component.hpp"

namespace LIA {
    class BaseGame : public Component {
        public:
            virtual std::string name() { return std::string{"Game"}; }
        protected:
            bool subscribe(std::string name) { return Component::subscribe(name); }
            bool subscribe(ComponentEvent event) { return Component::subscribe(event); }
    };
}
#endif