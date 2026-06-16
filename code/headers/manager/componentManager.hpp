#ifndef LIA_COMPONENT_MANAGER_HPP
#define LIA_COMPONENT_MANAGER_HPP
#include <vector>

namespace LIA {
    class Component;
    class ComponentManager {
        public:
            bool init();
            int addAndInit(Component*);
            Component* getComponent(int);
        private:
            std::vector<Component*> _data;
    };
}
#endif