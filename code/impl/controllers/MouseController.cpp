#include "controllers/MouseController.hpp"

void LIA::MouseController::setMouseClick(int button, float x, float y) {
    _x = x;
    _y = y;
    switch (button) {
    case 0: {
        _buttonL._clicked = true;
        _buttonL._isHeld = true;
    }break;
    case 1: {
        _buttonR._clicked = true;
        _buttonR._isHeld = true;
    }break;
    }
}
void LIA::MouseController::setMouseRelease(int button, float x, float y) {
    _x = x;
    _y = y;
    switch (button) {
    case 0: {
        if (_buttonL._clicked) {
            _buttonL._wasClicked = true;
        }
        _buttonL._clicked = false;
        _buttonL._isHeld = false;
    }break;
    case 1: {
        if (_buttonR._clicked) {
            _buttonR._wasClicked = true;
        }
        _buttonR._clicked = false;
        _buttonR._isHeld = false;
    }break;
    }
}
bool LIA::MouseController::wasMouseClicked() {
    return _buttonL._wasClicked && !_buttonL._clicked;
}
bool LIA::MouseController::isMouseClicked() {
    return _buttonL._clicked;
}
bool LIA::MouseController::isHeld() {
    return _buttonL._isHeld;
}
bool LIA::MouseController::wasRMouseClicked() {
    return _buttonR._wasClicked && !_buttonR._clicked;
}
bool LIA::MouseController::isRMouseClicked() {
    return _buttonR._clicked;
}
bool LIA::MouseController::isRHeld() {
    return _buttonR._isHeld;
}
void LIA::MouseController::resetState() {
    _buttonL._wasClicked = false;
    _buttonL._isHeld = _buttonL._clicked;

    _buttonR._wasClicked = false;
    _buttonR._isHeld = _buttonR._clicked;
}