#ifndef LIA_MOUSE_CONTROLLER_HPP
#define LIA_MOUSE_CONTROLLER_HPP
#include <string>
namespace LIA {
    class MouseController {
        private:
            struct Button {
                bool _clicked;
                bool _wasClicked;
                bool _isHeld;
            };
            Button _buttonL;
            Button _buttonR;
            float _x, _y;
        public:
            void setMouseClick(int button, float x, float y);
            void setMouseRelease(int button, float x, float y);
            bool wasMouseClicked();
            bool isMouseClicked();
            bool isHeld();
            bool wasRMouseClicked();
            bool isRMouseClicked();
            bool isRHeld();
            void resetState();
    };
}
#endif