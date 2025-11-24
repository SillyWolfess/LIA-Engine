#include "logs.hpp"
#include "component/Component.hpp"
#include "manager/eventManager.hpp"
#include "Engine.hpp"

bool LIA::Component::initCore() {
    Engine &engine = LIA::Engine::getInstance();
    _objectManager = &engine.getObjectManager();
    _eventManager = &engine.getEventManager();
    _componentManager = &engine.getComponentManager();
    _guiManager = &engine.getGuiManager();
    _textureManager = &engine.getTextureManager();
    _keybindingManager = &engine.getKeybindingManager();
    _identifier = -1;
    if (!init()) {
        return false;
    }
    if (!_registerHandlers()) {
        LIA_fatal("Failed to register handlers");
        return false;
    }
    return true;
};

bool LIA::Component::_registerHandlers() {
    return registerHandlers();
}

bool LIA::Component::subscribe(std::string name) {
    if (name.compare("load") == 0) {
        getEventManager()->subscribe("load", LIA::EventType::SIMULATION, std::bind(&Component::loadEventHandler, this, std::placeholders::_1));
        return true;
    }
    else if (name.compare("tick") == 0) {
        getEventManager()->subscribe("tick", LIA::EventType::SIMULATION, std::bind(&Component::tickEvenHandler, this, std::placeholders::_1));
        return true;
    }
    else if (name.compare("get_data_gui") == 0) {
        getEventManager()->subscribe("get_data_gui", LIA::EventType::GUI, std::bind(&Component::getDataGuiEventHandler, this, std::placeholders::_1));
        return true;
    }
    else if (name.compare("button_action") == 0) {
        getEventManager()->subscribe("button_action", LIA::EventType::GUI, std::bind(&Component::buttonActionEventHandler, this, std::placeholders::_1));
        return true;
    }
    else if (name.compare("init_gui_window") == 0) {
        getEventManager()->subscribe("init_gui_window", LIA::EventType::GUI, std::bind(&Component::guiWindowInitHandler, this, std::placeholders::_1));
        return true;
    }
    else {
        LIA_error_f("Subscription name '{}' is not known to engine", name);
        return false;
    }
}

void LIA::Component::setIdentifier(int identifier) {
    _identifier = identifier;
}

bool LIA::Component::tickEvenHandler(LIA::Event& event) {
     if (event.name.compare("tick") != 0) {
        return false;
    }
    return onTick(event);
}

bool LIA::Component::loadEventHandler(LIA::Event& event) {
     if (event.name.compare("load") != 0) {
        return false;
    }
    return onLoad(event);
}

bool LIA::Component::getDataGuiEventHandler(LIA::Event& event) {
    if (event.name.compare("get_data_gui") != 0) {
        return false;
    }
    return onGetGuiData(event);
}

bool LIA::Component::buttonActionEventHandler(LIA::Event& event) {
    if (event.name.compare("button_action") != 0) {
        return false;
    }
    return onButtonAction(event);
}

bool LIA::Component::guiWindowInitHandler(LIA::Event& event) {
    if (event.name.compare("init_gui_window") != 0) {
        return false;
    }
    return onGuiWindowInit(event);
}

bool LIA::Component::onTick(LIA::Event& event) {
    return false;
}

bool LIA::Component::onLoad(LIA::Event& event) {
    return false;
}

bool LIA::Component::onGetGuiData(LIA::Event& event) {
    return false;
}

bool LIA::Component::onButtonAction(LIA::Event& event) {
    return false;
}

bool LIA::Component::onGuiWindowInit(LIA::Event& event) {
    return false;
}

bool LIA::Component::registerHandlers() {
    return true;
};

bool LIA::Component::init() {
    return true;
}

bool LIA::Component::afterInit() {
    return true;
}