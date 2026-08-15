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
    _appWindow = &engine.getAppWindow();
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

bool LIA::Component::subscribe(LIA::ComponentEvent event) {
    if (event == ComponentEvent::LOAD) {
        getEventManager()->subscribe("load", LIA::EventType::SIMULATION, __FILE__, std::bind(&Component::loadEventHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::TICK) {
        getEventManager()->subscribe("tick", LIA::EventType::SIMULATION, __FILE__, std::bind(&Component::tickEvenHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::GET_DATA_GUI) {
        getEventManager()->subscribe("get_data_gui", LIA::EventType::GUI, __FILE__, std::bind(&Component::getDataGuiEventHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::BUTTON_ACTION) {
        getEventManager()->subscribe("button_action", LIA::EventType::GUI, __FILE__, std::bind(&Component::buttonActionEventHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::INIT_GUI_WINDOW) {
        getEventManager()->subscribe("init_gui_window", LIA::EventType::GUI, __FILE__, std::bind(&Component::guiWindowInitHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::CHECKBOX_ACTION) {
        getEventManager()->subscribe("checkbox_action", LIA::EventType::GUI, __FILE__, std::bind(&Component::checkboxEventHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::POSITION_CHANGED) {
        getEventManager()->subscribe("position_changed", LIA::EventType::SIMULATION, __FILE__, std::bind(&Component::positionChangedEventHandler, this, std::placeholders::_1));
        return true;
    } else if (event == ComponentEvent::OPTION_ACTION) {
        getEventManager()->subscribe("option_action", LIA::EventType::GUI, __FILE__, std::bind(&Component::optionChangedHandler, this, std::placeholders::_1));
    } else if (event == ComponentEvent::TIMER) {
        getEventManager()->subscribe("timer", LIA::EventType::GUI, __FILE__, std::bind(&Component::timerHandler, this, std::placeholders::_1));
    } else {
        LIA_error("Subscription event unknown");
        return false;
    }
}

bool LIA::Component::subscribe(std::string name) {
    if (name.compare("load") == 0) {
        return subscribe(ComponentEvent::LOAD);
    }
    else if (name.compare("tick") == 0) {
        return subscribe(ComponentEvent::TICK);
    }
    else if (name.compare("get_data_gui") == 0) {
        return subscribe(ComponentEvent::GET_DATA_GUI);
    }
    else if (name.compare("button_action") == 0) {
        return subscribe(ComponentEvent::BUTTON_ACTION);
    }
    else if (name.compare("init_gui_window") == 0) {
        return subscribe(ComponentEvent::INIT_GUI_WINDOW);
    }
    else if (name.compare("checkbox_action") == 0) {
        return subscribe(ComponentEvent::CHECKBOX_ACTION);
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

bool LIA::Component::checkboxEventHandler(LIA::Event& event) {
    if (event.name.compare("checkbox_action") != 0) {
        return false;
    }
    return onCheckboxAction(event);
}

bool LIA::Component::positionChangedEventHandler(LIA::Event& event) {
    if (event.name.compare("position_changed") != 0) {
        return false;
    }
    return onPositionChanged(event);
}

bool LIA::Component::optionChangedHandler(LIA::Event& event) {
    if (event.name.compare("option_action") != 0) {
        return false;
    }
    return onOptionChanged(event);
}

bool LIA::Component::timerHandler(LIA::Event& event) {
    if (event.name.compare("timer") != 0) {
        return false;
    }
    return onTimer(event);
}

bool LIA::Component::onTimer(LIA::Event& event) {
    return false;
}

bool LIA::Component::onTick(LIA::Event& event) {
    return false;
}

bool LIA::Component::onOptionChanged(LIA::Event& event) {
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

bool LIA::Component::onCheckboxAction(LIA::Event& event) {
    return false;
}

bool LIA::Component::onPositionChanged(LIA::Event& event) {
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