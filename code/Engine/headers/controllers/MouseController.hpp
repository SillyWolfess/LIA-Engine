#ifndef LIA_MOUSE_CONTROLLER_HPP
#define LIA_MOUSE_CONTROLLER_HPP
#include <string>
namespace LIA {
    class MouseController {
        private:
            float _x, _y;
            bool _clicked;
            bool _wasClicked;
            bool _isHeld;
        public:
            void setMouseClick(float x, float y) {
                _x = x;
                _y = y;
                _clicked = true;
                _isHeld = true;
            }
            void setMouseRelease(float x, float y) {
                if (_clicked) {
                    _wasClicked = true;
                }
                _x = x;
                _y = y;
                _clicked = false;
                _isHeld = false;
            }
            bool wasMouseClicked() {
                return _wasClicked && !_clicked;
            }
            bool isMouseClicked() {
                return _clicked;
            }
            bool isHeld() {
                return _isHeld;
            }
            void resetState() {
                _wasClicked = false;
                _isHeld = _clicked;
            }
    };
}
#endif