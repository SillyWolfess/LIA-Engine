#include "gui/gui.hpp"
#include "precompiled.hpp"
#include "Engine.hpp"

#define GUI_DEBUG_ON 0
bool LIA::Gui::loadStyle() {
    XmlLoader xmlLoader;
    LIA_TRY
        LIA_info("Load style");
        XmlLoader::XmlData styleData = xmlLoader.load("./data/gui/style.xml");
        LIA_trace("Loading style header");
        style.headerColor = xmlLoader.getColor(styleData, "header");
        LIA_trace("Loading header hover color");
        style.headerHoverColor = xmlLoader.getColor(styleData, "headerHover");
        LIA_trace("Loading header grabbed color");
        style.headerGrabbedColor = xmlLoader.getColor(styleData, "headerGrabbed");
        LIA_trace("Loading style hoverButtonBgColor");
        style.hoverButtonBgColor = xmlLoader.getColor(styleData, "hoverButtonBgColor");
        LIA_trace("Loading buttonBgColor");
        style.buttonBgColor = xmlLoader.getColor(styleData, "buttonBgColor");
        LIA_trace("Loading bgColor");
        style.bgColor = xmlLoader.getColor(styleData, "bgColor");
        LIA_trace("Loading disbaledButtonColor");
        style.disbaledButtonColor = xmlLoader.getColor(styleData, "disbaledButtonColor");
        LIA_trace("Loading hoverColor");
        style.hoverColor = xmlLoader.getColor(styleData, "hoverColor");
        LIA_trace("Loading fontSize");
        style.fontSize = xmlLoader.getInt(styleData, "fontSize");
        LIA_trace("Loading padding");
        style.padding = xmlLoader.getPadding(styleData);
        LIA_trace("Loading checkbox scale");
        style.checkBoxScale = xmlLoader.getScale2D(styleData, "checkBoxScale");
        LIA_trace("Loading checkbox checked color");
        style.checkBoxCheckedColor = xmlLoader.getColor(styleData, "checkBoxCheckedColor");
        LIA_trace("Loading checkbox unckecked color");
        style.checkBoxUnCheckedColor = xmlLoader.getColor(styleData, "checkBoxUnCheckedColor");
        LIA_trace("Loading checkbox hover color");
        style.checkBoxHoverColor = xmlLoader.getColor(styleData, "checkBoxHoverColor");
        return true;
    LIA_CATCH_RETURN_FALSE
}
bool LIA::Gui::init() {
    LIA_info("Init Gui");
    _appWindow = &(Engine::getInstance().getAppWindow());
    _eventManager = &(Engine::getInstance().getEventManager());

    if (!loadStyle()) {
        LIA_fatal("Failed to load style");
        return false;
    }

    LIA_debug("Loading gui data");
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load("./data/gui/gui.xml");

    LIA_TRY
    for (auto [name, path] : xmlData.values) {
        loadWindow(name, path);
    }
    LIA_CATCH_RETURN_FALSE
    #if GUI_DEBUG_ON
    int infoId = add("last key pressed");
    Data* infoData = &_data.at(infoId);
    setDefaults(infoData);
    setPosition(infoData, 300, 50);

    int mouseId = add("mouse position");
    Data* mouseData = &_data.at(mouseId);
    setDefaults(mouseData);
    setPosition(mouseData, 500, 100);
    #endif

    if (!registerHandlers()) {
        LIA_fatal("Failed to register gui handlers");
        return false;
    }
    _lastAppScale = _appWindow->getWindowScale();
    _watcher.subscribe("./data/gui/style.xml");
    _watcher.subscribe("./data/gui/gui.xml");
    return true;
}

bool LIA::Gui::loadWindow(Window& window, std::string path, bool initShow) {
    std::string name = window.getName();
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlWindow = xmlLoader.load(path);
        std::map<std::string, std::string> xmlWindowData = xmlWindow.values;
        window.setName(xmlWindowData.at("name"));
        window.setId(xmlWindowData.at("id"));
        window.setPath(path);
        LIA_trace("Setting style");
        window.setStyle(style);

        std::map<std::string, XmlLoader::XmlNode> nodes = xmlWindow.nodes;
        LIA_trace("Setting header");
        XmlLoader::XmlNode header = nodes.at("header");
        float headerSize = xmlLoader.getSize(header);
        bool headerVisible = xmlLoader.getBoolean(header, "visible");
        window.setHeader(headerVisible, headerSize);
        LIA_trace("Setting position");
        XmlLoader::XmlNode positionNode = nodes.at("position");
        Position position = xmlLoader.getPosition(positionNode);
        window.setPosition(position);
        LIA_trace("Setting scale");
        XmlLoader::XmlNode scaleNode = nodes.at("scale");
        Scale scale = xmlLoader.getScale2D(scaleNode);
        window.setScale(scale);
        LIA_trace("Setting button scale");
        XmlLoader::XmlNode buttonScaleNode = nodes.at("buttonScale");
        Scale buttonScale = xmlLoader.getScale2D(buttonScaleNode);
        window.setButtonScale(buttonScale);
        LIA_trace("Setting visibility");
        bool isVisible = xmlLoader.getBoolean(xmlWindow, "visible");
        LIA_trace("Setting initial align");
        std::string initAlign = xmlLoader.getString(xmlWindow, "initAlign", "none");
        window.setInitAlign(initAlign);
        LIA_trace("Setting movable");
        bool isMovable = xmlLoader.getBoolean(xmlWindow, "movable", false);
        window.setMovable(isMovable);
        LIA_trace("Setting alignment");
        std::string alignment = xmlLoader.getString(xmlWindow, "align", "none");
        window.setAlignment(alignment);
        LIA_trace("Loading children");
        XmlLoader::XmlNode childrenNode = nodes.at("children");
        std::list<XmlLoader::XmlNode> chidlren = childrenNode.children;
        for (auto node: chidlren) {
            LIA_trace("Loading child");
            std::string childType = xmlLoader.getString(node, "type");
            if (childType.compare("button") == 0) {
                LIA_trace("Adding button");
                std::string buttonId = xmlLoader.getString(node, "id");
                std::string buttonName = xmlLoader.getString(node, "name");
                std::string eventAction = xmlLoader.getString(node, "action", "");
                std::string eventArg0 = xmlLoader.getString(node, "arg0", "");
                if (buttonId.compare("") == 0) {
                    LIA_fatal("Button id is empty for {}", std::make_format_args(buttonName));
                    return false;
                }
                window.addButton(buttonId, buttonName, eventAction, eventArg0);
            }
            else if (childType.compare("field") == 0) {
                LIA_trace("Adding field");
                std::string fieldId = xmlLoader.getString(node, "id");
                std::string fieldLabel = xmlLoader.getString(node, "label");
                std::string fieldPlaceholder = xmlLoader.getString(node, "placeholder", "");
                window.addField(fieldId, fieldLabel, "", fieldPlaceholder);
            }
            else if (childType.compare("checkbox") == 0) {
                LIA_trace("Adding checkbox");
                std::string id = xmlLoader.getString(node, "id");
                std::string eventAction = xmlLoader.getString(node, "action");
                std::string label = xmlLoader.getString(node, "label");
                bool isChecked = xmlLoader.getBoolean(node, "checked", false);
                window.addCheckBox(id, label, isChecked, eventAction);
            }
            else if (childType.compare("label") == 0) {
                LIA_trace("Adding label");
                std::string id = xmlLoader.getString(node, "id");
                std::string value = xmlLoader.getString(node, "value");
                window.addLabel(id, value);
            }
            else if (childType.compare("grid") == 0) {
                LIA_trace("Adding grid");
                int minRows = xmlLoader.getInt(node, "minRows", 1);
                int maxRows = xmlLoader.getInt(node, "maxRows", 1);
                int minColumns = xmlLoader.getInt(node, "minColumns", 1);
                int maxColumns = xmlLoader.getInt(node, "maxColumns", 1);
                std::string id = xmlLoader.getString(node, "id");
                std::string name = xmlLoader.getString(node, "name");
                window.addGrid(id, name, minRows, maxRows, minColumns, maxColumns);
            }
            else if(childType.compare("list") == 0) {
                LIA_trace("Adding list");
                std::string id = xmlLoader.getString(node, "id");
                std::string name = xmlLoader.getString(node, "name");
                window.addList(id, name);
            }
            else {
                LIA_error(std::vformat("Unknown child type {} in {}", std::make_format_args(childType, name)));
            }
        }
        if (initShow) {
            if (isVisible) {
                window.show(_appWindow);
            } else {
                window.hide();
            }
        }
        return true;
}

bool LIA::Gui::loadWindow(std::string name, std::string path) {
    LIA_TRY
        Window window;
        if (!loadWindow(window, path, true)) {
            return false;
        }
        _windows.emplace_back(window);
        int indx = _windows.size() - 1;
        _windowMap.emplace(std::pair<std::string, int>(window.getId(), indx));
        _pathMap.emplace(std::pair<std::string, std::string>(name, path));
        _watcher.subscribe(path);
        LIA_debug_f("New window {} with id", window.getId(), indx);
        return true;
    LIA_CATCH_RETURN_FALSE
}

void LIA::Gui::toggleWindow(std::string id) {
    Window *window = getWindow(id);
    if (window == nullptr) {
        LIA_warn_f("No window found for id '{}'", id);
        return;
    }
    if (window->isVisible()) {
        closeWindow(window);
    } else {
        openWindow(window);
    }
}

void LIA::Gui::closeWindow(Window* window) {
    LIA_ASSERT(window != nullptr, "window is nullptr");
    window->hide();
}

void LIA::Gui::openWindow(Window* window) {
    LIA_ASSERT(window != nullptr, "window is nullptr");
    window->show(_appWindow);
    GuiGetDataEvent guiGetDataEvent(window->getId());
    _eventManager->handleEvent(guiGetDataEvent);
}

void LIA::Gui::closeWindow(std::string id) {
    Window *window = getWindow(id);
    if (window == nullptr) {
        LIA_warn_f("No window found for id '{}'", id);
        return;
    }
    closeWindow(window);
}

void LIA::Gui::openWindow(std::string id) {
    Window *window = getWindow(id);
    if (window == nullptr) {
        LIA_warn_f("No window found for id '{}'", id);
        return;
    }
    openWindow(window);
}

bool LIA::Gui::handleGuiButton(Event& event) {
    if (event.name.compare("button_action") != 0) {
        return false;
    }
    if (event.action.compare("open_window_and_close") == 0) {
        if (event.arg0.compare("") == 0) {
            LIA_error(std::vformat("No argument for open_window_and_close from {}", std::make_format_args(event.source)));
            return false;
        }
        if (event.source.compare("") == 0) {
            LIA_error(std::vformat("No source for open_window_and_close from {}", std::make_format_args(event.source)));
            return false;
        }
        openWindow(event.arg0);
        closeWindow(event.source);
        return true;
    }
    else if (event.action.compare("open_window") == 0) {
        if (event.arg0.compare("") == 0) {
            LIA_error(std::vformat("No argument for open_window from {}", std::make_format_args(event.source)));
            return false;
        }
        openWindow(event.arg0);
        return true;
    }
    else if (event.action.compare("close_window") == 0) {
        if (event.arg0.compare("") == 0) {
            LIA_error(std::vformat("No argument for close_window from {}", std::make_format_args(event.source)));
            return false;
        }
        closeWindow(event.arg0);
        return true;
    }
    else if (event.action.compare("toggle_window") == 0) {
        if (event.arg0.compare("") == 0) {
            LIA_error_f("No argument for toggle_window from {}", event.source);
            return false;
        }
        toggleWindow(event.arg0);
        return true;
    }
    return false;
}
bool LIA::Gui::handleEnableEvent(Event& event) {
    if (event.name.compare("set_enabled") != 0) {
        return false;
    }
    Window *window = getWindow(event.window);
    if (window != nullptr) {
        LIA_trace_f("Setting enabled {}.{} = {}", event.window, event.target, event.argb);
        window->enableField(event.target, event.argb);
        return true;
    }
    return false;
}
bool LIA::Gui::handleGuiUpdate(Event& event) {
    if (event.name.compare("fps") != 0 && event.name.compare("update_gui") != 0) {
        return false;
    }
    if (event.name.compare("update_gui") == 0) {
        Window *window = getWindow(event.window);
        if (window != nullptr) {
            if (event.useList) {
                LIA_trace_f("Updating {}.{} = [list.size={}]", event.window, event.target, event.argl.size());
                window->updateList(event.target, event.argl, event.argi);
            } else {
                LIA_trace_f("Updating {}.{} = {}", event.window, event.target, event.arg0);
                window->updateField(event.target, event.arg0);
            }
            return true;
        }
        
        LIA_warn(std::vformat("Action {} not recognized from {}", std::make_format_args(event.action, event.source)));
        return false;
    }
    if (event.action.compare("fps") == 0) {
        Window *window = getWindow("debug");
        if (window != nullptr) {
            if (!window->isVisible()) {
                return false;
            }
            window->updateField("fps", std::to_string(event.argi));
            return true;
        }
    }
    else {
        LIA_warn(std::vformat("Action {} not recognized from {}", std::make_format_args(event.action, event.source)));
    }
    return false;
}

bool LIA::Gui::registerHandlers() {
    _eventManager->subscribe("button_action", EventType::GUI, std::bind(&Gui::handleGuiButton, this, std::placeholders::_1));
//    eventManager.subscribe("checkbox_action", EventType::GUI, std::bind(&Gui::handleCheckbox, this, std::placeholders::_1));
    _eventManager->subscribe("fps", EventType::GUI, std::bind(&Gui::handleGuiUpdate, this, std::placeholders::_1));
    _eventManager->subscribe("update_gui", EventType::GUI, std::bind(&Gui::handleGuiUpdate, this, std::placeholders::_1));
    _eventManager->subscribe("set_enabled", EventType::GUI, std::bind(&Gui::handleEnableEvent, this, std::placeholders::_1));
    return true;
}

void LIA::Gui::setDefaults(Data* data) {
    data->color.r = 1.0f;
    data->color.g = 1.0f;
    data->color.b = 1.0f;

    data->size = 20;
}

void LIA::Gui::setPosition(Data* data, float x, float y) {
    data->position.x = x;
    data->position.y = y;
    data->position.z = 0.0f;
}

int LIA::Gui::add(std::string text) {
    Data data;
    data.text = text;
    _data.emplace_back(data);
    return _data.size() - 1;
} 

void LIA::Gui::update() {
    if (_watcher.needsReload("./data/gui/gui.xml")) {
        LIA_TRY
            XmlLoader xmlLoader;
            XmlLoader::XmlData xmlData = xmlLoader.load("./data/gui/gui.xml");

            for (auto [name, path] : xmlData.values) {
                if (_pathMap.find(name) == _pathMap.end()) {
                    if (!loadWindow(name, path)) {
                        LIA_error_f("Failed to load {} from '{}'", name, path);
                    }
                }
            }
        LIA_CATCH_EMPTY
    }
    if (_watcher.needsReload("./data/gui/style.xml")) {
        if (!loadStyle()) {
            LIA_error("Failed to reload style");
        } else {
            LIA_trace("Style reloaded");
            for (Window& window: _windows) {
                window.setStyle(style);
            }
        }
    }

    for (Window& window: _windows) {
        LIA_TRY
        if (_watcher.needsReload(window.getPath())) {
            Window tmpWindow;
            tmpWindow.setPath(window.getPath());
            if (loadWindow(tmpWindow, tmpWindow.getPath(), false)) {
                window.clear();
                if (loadWindow(window, window.getPath(), false)) {
                    window.resize(_appWindow);
                    if (window.isVisible()) {
                        GuiGetDataEvent guiGetDataEvent(window.getId());
                        _eventManager->handleEvent(guiGetDataEvent);
                    }
                    GuiInitEvent guiInitEvent(window.getId());
                    _eventManager->handleEvent(guiInitEvent);
                }
            } else {
                LIA_error_f("Failed to reload window {}", window.getName());
            }
        }
        LIA_CATCH_EMPTY
    }

    AppWindow* appWindow = _appWindow;
    EventManager* eventManager = _eventManager;
    int lastKey = appWindow->lastKeyPressed();
    #if GUI_DEBUG_ON
    if (lastKey != -1) {
        Data* info = &_data.at(0);
        info->text = appWindow->getKeyName(lastKey);
    }
    #endif
    Scale appScale = appWindow->getWindowScale();
    bool isFullscreen = appWindow->isFullscreen();
    if (appScale.x != _lastAppScale.x || appScale.y != _lastAppScale.y || isFullscreen != _lastFullscreen) {
        for (Window& window: _windows) {
            window.resize(appWindow);
        }
    }
    if (appWindow->isCursorInsideWindow()) {
        Position mousePos = appWindow->getMousePos();
        bool mouseClicked = appWindow->wasMouseClicked();
        bool mouseHeld = appWindow->isMouseHeld();
        bool mouseDown = appWindow->isMousePressed();

        #if GUI_DEBUG_ON
        Data* mouseInfo = &_data.at(1);
        std::string mouseString = std::vformat("mouse {} x {}", std::make_format_args(mousePos.x, mousePos.y));
        mouseInfo->text = mouseString;
        #endif

        bool mouseClickFound = false;
        bool windowGrabbed = false;
        _lastGrabbedWindow = -1;
        for (int i = 0; i < _windows.size(); i++) {
            if (_windows[i].isGrabbed()) {
                _lastGrabbedWindow = i;
                break;
            }
        }
        for (Window& window: _windows) {
            if (!mouseClickFound && mouseClicked) {
                mouseClickFound = window.mouseClick(mousePos, eventManager);
            } else {
                window.mouseHover(mousePos);
            }
            if (_lastGrabbedWindow == -1 && mouseDown && !windowGrabbed) {
                windowGrabbed = window.grab(mousePos);
            }
        }
        if ((windowGrabbed || _lastGrabbedWindow != -1) && mouseHeld) {
            for (Window& window: _windows) {
                if (window.mouseDown(mousePos, eventManager)) {
                    break;
                }
            }
        }
        for (Window& window: _windows) {
           window.mouseLastPosition(mousePos);
        }
    }
    _lastAppScale = appScale;
    _lastFullscreen = isFullscreen;
}

LIA::Gui::~Gui() {
    LIA_trace("Destroying Gui");
}

void LIA::Gui::passObjects(Scene* scene, Font* font) {
    for (Data& data: _data) {
        font->addText(data.text, data.position, data.color, data.size);
    }
    for (Window& window: _windows) {
        window.passObjects(scene, font);
    }
}

bool LIA::Gui::registerButton(std::string target, std::string id, std::string value, std::string action, std::string arg0) {
    Window* window = getWindow(target);
    if (window == nullptr) {
        return false;
    }
    window->addButton(id, value, action, arg0);
    return true;
}

#define LIA_GET_WINDOW_USE_FOR_LOOP 0
LIA::Window* LIA::Gui::getWindow(std::string id) {
    #if LIA_GET_WINDOW_USE_FOR_LOOP == 0
        if (_windowMap.find(id) == _windowMap.end()) {
            return nullptr;
        } 
        int indx = _windowMap[id];
        if (indx < 0 || indx >= _windows.size()) {
            return nullptr;
        }
        if (_windows[indx].getId().compare(id) == 0) {
            return &_windows[indx];
        }
        return nullptr;
    #else
        for (Window& window : _windows) {
            if (window.getId().compare(id) == 0) {
                return &window;
            }
        }
        return nullptr;
    #endif
}