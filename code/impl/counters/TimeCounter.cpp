#include "counters/TimeCounter.hpp"
#include "data/Event.hpp"
#include "manager/eventManager.hpp"
#include <GLFW/glfw3.h>

void LIA::TimeCounter::setName(std::string name) {
    _name = name;
}

void LIA::TimeCounter::notify(EventManager& eventManager) {
    TimerEvent event(_name, _deltaTime);
    eventManager.handleEvent(event);
}

void LIA::TimeCounter::end() {
    double currentTime = glfwGetTime();
    _deltaTime = currentTime - _lastTime;
}

void LIA::TimeCounter::start() {
    _lastTime = glfwGetTime();
}