#ifndef LIA_TIME_COUNTER
#define LIA_TIME_COUNTER

#include <string>

namespace LIA {
    class EventManager;
    class TimeCounter {
        public:
            void setName(std::string);
            void start();
            void end();
            void notify(EventManager&);
            double getDelta() { return _deltaTime; };
        private:
            std::string _name;
            double _lastTime;
            double _deltaTime;
    };
}
#endif