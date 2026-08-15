#include "counters/FpsCounter.hpp"
#include "data/Event.hpp"
#include "manager/eventManager.hpp"

void LIA::FpsCounter::update(double currentTime, EventManager& eventManager) {
    _deltaTime = currentTime - _lastTime;
    _nFrames++;
    if (_deltaTime >= 1.0) {
        if (_fps != _nFrames) {
            _fps = _nFrames;
            FpsEvent event(_fps);
            eventManager.handleEvent(event);
        }
        _nFrames = 0;
        _lastTime = currentTime;
    }
}