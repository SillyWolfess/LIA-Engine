#include "precompiled.hpp"
#include "Engine.hpp"

#include "logs.hpp"

bool LIA::Engine::init() {
    LIA_info("Init Engine");

    if (!_eventManager.init()) {
        LIA_fatal("Event manager init failed");
        return false;
    }
    LIA_trace("Event manager intialized");    

    if (!_window.init(_game->name())) {
        LIA_fatal("window init failed");
        return false;
    }
    LIA_trace("Window initialized");

    if (!_shaderManager.init()) {
        LIA_fatal("shader init failed");
        return false;
    }
    LIA_trace("ShaderManager initialized");

    if (!_textureManager.init()) {
        LIA_fatal("TextureManager init failed");
        return false;
    }
    LIA_trace("TextureManager initialized");

    if (!_window.initPrefab()) {
        LIA_fatal("window init prefabs failed");
        return false;
    }
    LIA_trace("Window prefabs initialised");

    GLuint fontId = _shaderManager.getProgramId("font");

    if (!_window.initFont(fontId)) {
        LIA_fatal("font init failed");
        return false;
    }
    LIA_trace("Font initialized");

    if (!_componentManager.init()) {
        LIA_fatal("Failed to init component manager");
        return false;
    }
    LIA_trace("Component manager initialized");

    if (!_simulation.init()) {
        LIA_fatal("simulation init failed");
        return false;
    }
    LIA_trace("Simulation initialized");

    if (!_gui.init()) {
        LIA_fatal("gui init failed");
        return false;
    }
    LIA_trace("Gui initialized");

    if (!registerEventHandlers()) {
        LIA_fatal("Failed to register event handlers");
        return false;
    }
    LIA_trace("Event handlers registered");
    return true;
}

bool LIA::Engine::initImplementation(Component* component) {
    LIA_info("Initializing implementation");

    if (_componentManager.addAndInit(component) == -1) {
        return false;
    }

    LIA_info("Implementation initialzed");
    return true;
}

bool LIA::Engine::handleGui(Event& event) {
    if (event.name.compare("button_action") != 0) {
        return false;
    }
    if (event.action.compare("exit") == 0) {
        return exit();
    }
    if (event.action.compare("start_simulation") == 0) {
        if (load()) {
            _simulation.unpause();
            if (_simulation.getState() != LIA_SIM_STATE::RUNNING) {
                LIA_error("Failed to start the simulation");
            } else {
                _gui.closeWindow("main_menu");
                _gui.openWindow("top_bar");
            }
            return true;
        }
        return exit();
    }
    if (event.action.compare("continue_simulation") == 0) {
        _simulation.unpause();
        if (_simulation.getState() != LIA_SIM_STATE::RUNNING) {
            LIA_error("Failed to unpause the simulation");
        } else {
            _gui.closeWindow("ingame_menu");
            _gui.openWindow("top_bar");
        }
        return true;
    }
    if (event.action.compare("open_main_menu") == 0) {
        _gui.closeWindow(event.window);
        _gui.openWindow("main_menu");
        return true;
    }
    if (event.action.compare("open_ingame_menu") == 0) {
        _simulation.pause();
        if (_simulation.getState() != LIA_SIM_STATE::PAUSED) {
            LIA_error("Failed to pause the simulation");
        } else {
            _gui.closeWindow("top_bar");
            _gui.openWindow("ingame_menu");
        }
        return true;
    }
    return false;
}

bool LIA::Engine::handleGuiGetData(Event& event) {
    if (event.name.compare("get_data_gui") != 0) {
        return false;
    }
    if (event.source.compare("debug") == 0) {
        UpdateGuiEvent updateGuiEvent(event.window, "fps", _fps);
        _eventManager.handleEvent(updateGuiEvent);
        return true;
    }
    return false;
}

bool LIA::Engine::handleGameState(Event& event) {
    if (event.name.compare("gameLost") != 0) {
        return false;
    }
    _simulation.setLost();
    _simulation.end();
    if (_simulation.getState() != LIA_SIM_STATE::ENDED) {
        LIA_error("Failed to end the simulation");
    } else {
        _gui.closeWindow("top_bar");
        _gui.openWindow(event.arg0);
    }
    return true;
}

bool LIA::Engine::exit() {
    LIA_trace("Closing application");
    _window.close();
    return true;
}

bool LIA::Engine::registerEventHandlers() {
    _eventManager.subscribe("button_action", EventType::GUI, std::bind(&Engine::handleGui, this, std::placeholders::_1));
    _eventManager.subscribe("get_data_gui", EventType::GUI, std::bind(&Engine::handleGuiGetData, this, std::placeholders::_1));
    _eventManager.subscribe("gameLost", EventType::SIMULATION, std::bind(&Engine::handleGameState, this, std::placeholders::_1));
    return true;
}

bool LIA::Engine::load() {
    if (!_textureManager.load()) {
        LIA_fatal("Failed to load basic textures");
        return false;
    }
    if (!_simulation.load(&_shaderManager)) {
        LIA_fatal("Failed to load simulation");
        return false;
    }
    if (_simulation.getState() != LIA_SIM_STATE::LOADED) {
        LIA_fatal("Failed to load simulation. State does not match");
        return false;
    }
    if (!_terrain.load()) {
        LIA_fatal("Failed to load terrain");
        return false;
    }
    if (_terrain.isEnabled()) {
        Scene* tScene = _window.getTerrain();
        if (!_terrain.pass(tScene)) {
            LIA_fatal("Failed to pass terrain to renderer");
            return false;
        }
    }
    LIA::LoadEvent loadEvent("game", -1);
    _eventManager.handleEvent(loadEvent);
    return true;
}

bool LIA::Engine::update() {
    double currentTime = glfwGetTime();
    _deltaTime = currentTime - _lastTime;
    _nFrames++;
    if (_deltaTime >= 1.0) {
        double value = 1000.0 / double(_nFrames);
        if (_fps != _nFrames) {
            _fps = _nFrames;
            FpsEvent event(_fps);
            _eventManager.handleEvent(event);
        }
        _nFrames = 0;
        _lastTime = currentTime;
    }

    _watcher.watch();
    _shaderManager.update();

    _window.update();
    if (!_simulation.update(&_window, _deltaTime)) {
        LIA_fatal("Simulation update failed");
        return false;
    }
    _gui.update();
    _window.resetState();

    draw();
    return true;
}

void LIA::Engine::draw() {
    Scene* scene = _window.getScene();
    Scene* guiScene = _window.getGui();
    Font* font = _window.getFont();

    scene->clear();
    guiScene->clear();
    font->clearText();

    _simulation.passObjects(scene);
    _gui.passObjects(guiScene, font);

    _window.draw();
}

bool LIA::Engine::isExit() {
    return _window.isExit();
}

LIA::Engine::Engine() {
    LIA_DEBUG_CONSTRUCTOR("Engine");
    LIA_trace("Constructor Engine");
}
LIA::Engine::~Engine() {
    LIA_DEBUG_DESTRUCTOR("Engine");
    LIA_trace("Destroying Engine");
}

void LIA::Engine::setGame(BaseGame* game) {
    LIA_debug("Setting game");
    _game = game;
}

#define LOG_TEST 0
void LIA::Engine::run() {
     LIA_TRY
        LIA_trace("Engine.run");
        #if LOG_TEST
            LIA_info("info");            
            LIA_info_f("info {} - {} = {}", 3 , 4, 3 - 4);
            LIA_warn("warn");
            LIA_debug("debug");
            LIA_debug_f("debug {} + {} = {}", 3, 4, 3 + 4);
            LIA_error("error");
            LIA_fatal("fatal");
            LIA_trace("trace");
        #else
            LIA_ASSERT(_game != nullptr, "game is not set");
            if (!init()) { 
                return;
            }
            if (!initImplementation(_game)) {
                LIA_fatal("Failed to init game component");
                return;
            }
            while (!isExit()) {
                if (!update()) {
                    return;
                }
            }
        #endif
    LIA_CATCH_EMPTY
    LIA_DEBUG_LOG
}

namespace LIA {
    Engine& Engine::getInstance() {
        static Engine engineInstance;
        return engineInstance;
    }
}