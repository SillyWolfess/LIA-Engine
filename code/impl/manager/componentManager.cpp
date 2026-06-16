#include "logs.hpp"
#include "manager/componentManager.hpp"
#include "component/Component.hpp"

bool LIA::ComponentManager::init() {
    return true;
}

int LIA::ComponentManager::addAndInit(Component* component) {
    if (!component->initCore()) {
        return -1;
    }
    _data.emplace_back(component);
    component->setIdentifier(_data.size() - 1);
    if (!component->afterInit()) {
        return -1;
    }
    return _data.size() - 1;
}


LIA::Component* LIA::ComponentManager::getComponent(int id) {
    LIA_trace_f("Get component of id {}", id);
    LIA_ASSERT(id > -1 , "Id is wrong");
    LIA_ASSERT(id < _data.size(), "Id is out of range");
    return _data[id];
}