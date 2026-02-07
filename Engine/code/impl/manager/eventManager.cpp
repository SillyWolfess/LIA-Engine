#include "manager/eventManager.hpp"
#include "logs.hpp"
#include "tools/macros.hpp"

bool LIA::EventManager::init() {
    LIA_ASSERT(!_isSet, "EventManager was already initialized");
    LIA_info("Event manager initialized");
    _isSet = true;
    return true;
}

LIA::EventManager::EventManager() : _isSet{false} {
    LIA_trace("EventManager constructor");
}

LIA::EventManager::~EventManager() {
    LIA_trace("EventManager destroyed");
}

void LIA::EventManager::handleEvent(Event& event) {
    LIA_ASSERT(_isSet, "HandleEvent called while eventManager is not initialized");

    LIA_debug(std::vformat("Handling event {}", std::make_format_args(event.name)));
    if (_eventHandlers.find(event.name) == _eventHandlers.end()) {
        LIA_warn(std::vformat("No event handler for {}", std::make_format_args(event.name)));
        return;
    }
    std::list<EventHandlers>& handlers = _eventHandlers[event.name];
    int i = 0;
    for (EventHandlers& handler : handlers) {
        i++;
        LIA_trace(std::vformat("Calling handler {} for event {} to {}", std::make_format_args(i, event.name, handler.callerName)));
        LIA_TRY
            if (event.type == EventType::GUI || event.type == EventType::ANY) {
                if (handler.guiHandler && handler.guiHandler(event)) {
                    LIA_debug(std::vformat("Event {} was handled by gui handler {} from {}", std::make_format_args(event.name, i, handler.callerName)));
                }
            }
        LIA_CATCH(LIA_error_f("Handling of the event {} failed by {}", event.name, handler.callerName);)
        LIA_TRY
            if (event.type == EventType::SIMULATION || event.type == EventType::ANY) {
                if (handler.simulationHandler && handler.simulationHandler(event)) {
                    LIA_debug(std::vformat("Event {} was handled by simulation handler {} from {}", std::make_format_args(event.name, i, handler.callerName)));
                }
            }
        LIA_CATCH(LIA_error_f("Handling of the event {} failed by {}", event.name, handler.callerName);)
    }
}

void LIA::EventManager::subscribe(std::string eventName, EventType type, std::string callerName, std::function<bool(Event&)> handler) {
    LIA_ASSERT(_isSet, "subscribe called while eventManager is not initialized");
    
    LIA_TRY
        LIA_debug(std::vformat("Adding handler for event {} from {} ", std::make_format_args(eventName, callerName)));
        if (_eventHandlers.find(eventName) == _eventHandlers.end()) {
            std::list<EventHandlers> handlersList;
            _eventHandlers.emplace(std::pair<std::string, std::list<EventHandlers>>(eventName, handlersList));
        }
        std::list<EventHandlers>& handlers = _eventHandlers[eventName];
        EventHandlers& ev = handlers.emplace_back();
        ev.callerName = callerName;
        ev.guiHandler = nullptr;
        ev.simulationHandler = nullptr;
        if (type == EventType::ANY || type == EventType::GUI) {
            LIA_debug_f("Adding handler as gui for {} from {}", eventName, callerName);
            ev.guiHandler = handler;
        }
        if (type == EventType::ANY || type == EventType::SIMULATION) {
            LIA_debug_f("Adding handler as simulation for {} from {}", eventName, callerName);
            ev.simulationHandler = handler;
        }
    LIA_CATCH(LIA_error_f("Subscribing to the event {} failed from {}", eventName, callerName);)
}

void LIA::EventManager::subscribe(std::string eventName, EventType type, std::function<bool(Event&)> handler) {
   subscribe(eventName, type, "anonymus", handler);
}