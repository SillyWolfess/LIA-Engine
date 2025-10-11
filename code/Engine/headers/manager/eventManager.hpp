#ifndef LIA_EVENT_MANAGER
#define LIA_EVENT_MANAGER

#include <map>
#include <string>
#include <list>
#include <functional>
#include "./../data/Event.hpp"

namespace LIA {
    class EventManager {
        private:
            struct EventHandlers {
                std::function<bool(Event&)> guiHandler;
                std::function<bool(Event&)> simulationHandler;
            };
            std::map<std::string, std::list<EventHandlers>> _eventHandlers;
            bool _isSet;
        public:
            EventManager();
            bool init();
            bool isSet() { return _isSet; }
            /**
             * Event - event that was triggered
             */
            void handleEvent(Event&);
            /**
             * std::string - event name
             * EventType - type of the event
             * std::function<bool(Event&)> - handler for the event
             */
            void subscribe(std::string, EventType, std::function<bool(Event&)> handler);
            ~EventManager();
    };
}
#endif