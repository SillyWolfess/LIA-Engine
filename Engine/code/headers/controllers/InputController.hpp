#ifndef LIA_KEY_CONTROLLER_HPP
#define LIA_KEY_CONTROLLER_HPP
#include <string>
namespace LIA {
    class KeyController {
        private:
            bool _guiKeys[500];
            int _lastKey = -1;
            std::string _keyNames[500];
            bool isKeyCodeValid(int);
        public:
            bool init();
            void setKeyPressed(int);
            void setKeyReleased(int);
            bool isKeyPressed(std::string, int);
            int getLastKeyPressed();
            std::string getKeyName(int);
    };
}
#endif