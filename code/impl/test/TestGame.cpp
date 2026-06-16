#include "logs.hpp"
#include "test/TestGame.hpp"

#include "manager/eventManager.hpp"
#include "Engine.hpp"

bool TEST_GAME::TestGame::init() {
    _eventSource = "list";
    if (!getGuiManager()->loadWindow(_eventSource, "./data/test/gui/list.xml")) {
        LIA_error_f("Failed to init window for {}", _eventSource);
        return false;
    }
    if (!getGuiManager()->registerButton("main_menu", _eventSource, "List", "toggle_window", _eventSource)) {
        LIA_error_f("Failed to add {} window to the main menu", _eventSource);
        return false;
    }
    LIA::EventManager* eventManager = getEventManager();
    LIA::SetGridFieldEnabledEvent enableButtonEvent("bottom_bar", "action", "action[0][0]", true);
    eventManager->handleEvent(enableButtonEvent);
//    LIA::Engine::getInstance().getTerrain().setEnabled(true);
    return true;
}

bool TEST_GAME::TestGame::registerHandlers() {
    if (!subscribe(LIA::ComponentEvent::GET_DATA_GUI)) {
        LIA_fatal("Failed to subscribe to gui update");
        return false;
    }
    if (!subscribe(LIA::ComponentEvent::INIT_GUI_WINDOW)) {
        LIA_fatal("Failed to subscribe to gui window creation");
        return false;
    }
    if (!subscribe(LIA::ComponentEvent::LOAD)) {
        LIA_fatal("Failed to subscribe to load event");
        return false;
    }
    return true;
}

bool TEST_GAME::TestGame::onLoad(LIA::Event& event) {
    if (event.source.compare("game") != 0) {
        return false;
    }
    LIA::ObjectManager* objectManager = getObjectManager();

    LIA_trace("registering data paths");
    if (!objectManager->registerPathsFromFile("./data/test/simulation/data.xml")) {
        LIA_fatal("Failed to register paths from file");
        LIA::Engine::getInstance().fatal();
        return true;
    }

    if (!objectManager->registerTemplatePathsFromFile("./data/test/objects/data.xml")) {
        LIA_fatal("Failed to register template paths from file");
        LIA::Engine::getInstance().fatal();
        return true;
    }

    if (!objectManager->getModelManager()->registerPathsFromFile("./data/test/models/data.xml")) {
        LIA_fatal("Failed to register model paths from file");
        LIA::Engine::getInstance().fatal();
        return true;
    }
    /***
     * Create what we need to
     */
    LIA_trace("Creating objects");
    if (!objectManager->createObject("player1", "player1")) {
        LIA_fatal("Failed to create player1");
        LIA::Engine::getInstance().fatal();
        return true;
    }

    if (!objectManager->createObject("snake", "snake")) {
        LIA_fatal("Failed to create snake");
        LIA::Engine::getInstance().fatal();
        return true;
    }
    return true;
}

bool TEST_GAME::TestGame::onGuiWindowInit(LIA::Event& event) {
    LIA_debug_f("onGuiWindowInit {}", event.source);
    if (event.source.compare("main_menu") == 0) {
        if (!getGuiManager()->registerButton("main_menu", _eventSource, "List", "toggle_window", _eventSource)) {
            LIA_error_f("Failed to add {} window to the main menu", _eventSource);
            return false;
        }
        return true;
    }
    if (event.source.compare("bottom_bar") == 0) {
        LIA::EventManager* eventManager = getEventManager();
        LIA::SetGridFieldEnabledEvent enableButtonEvent(event.source, "action", "action[0][0]", true);
        eventManager->handleEvent(enableButtonEvent);
        return true;
    }
    return false;
}

bool TEST_GAME::TestGame::onGetGuiData(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    
    updateData(event.window);
    return true;
}

void TEST_GAME::TestGame::updateData(std::string window) {
    LIA_TRY
        std::vector<std::string> fixedList;
        fixedList.push_back("a");
        fixedList.push_back("b");
        fixedList.push_back("c");
        LIA::UpdateGuiListEvent updateEvent(window, _eventSource, fixedList, -1);
        getEventManager()->handleEvent(updateEvent);
    LIA_CATCH_EMPTY
}