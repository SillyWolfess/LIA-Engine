#ifndef LIA_KEY_CONTROLLER_HPP
#define LIA_KEY_CONTROLLER_HPP
#include <string>
#define MAX_KEYS 500
namespace LIA {
    class KeyController {
        private:
            bool _guiKeys[MAX_KEYS];
            int _lastKey = -1;
            std::string _keyNames[MAX_KEYS];
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