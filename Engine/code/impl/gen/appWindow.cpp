#include "appWindow.hpp"
#include "precompiled.hpp"
#include <string>
#include <format>
#include <map>
#include "Engine.hpp"

int LIA::AppWindow::_cpX = 0;
int LIA::AppWindow::_cpY = 0;
int LIA::AppWindow::_offsetCpX = 0;
int LIA::AppWindow::_offsetCpY = 0;
int LIA::AppWindow::_buttonEvent = 0;
LIA::KeyController LIA::AppWindow::_keyController = KeyController();
LIA::MouseController LIA::AppWindow::_mouseController = MouseController();

void error_callback_glfw(int error, const char* description) {
    LIA_error(std::vformat("GLFW ERROR: code {} msg: {}.", std::make_format_args( error, description)));
}

void cursor_position_callback(GLFWwindow* window, double x, double y){
    if (LIA::AppWindow::getButtonEvent() == 1) {
        LIA::AppWindow::calculateOffset(x, y);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!glfwGetWindowAttrib(window, GLFW_HOVERED)) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            LIA::AppWindow::setButtonEvent(1);
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            LIA::AppWindow::setCp(floor(x), floor(y));
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            LIA::AppWindow::setButtonEvent(0);
            LIA::AppWindow::setCp(0, 0);
        }
    } else {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            LIA::AppWindow::mouseClick(0, x, y);
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            LIA::AppWindow::mouseRelease(0, x, y);
        }
    }
}
/*
void window_size_callback(GLFWwindow* window, int width, int height) {
    LIA_trace_f("Window resizing: {} x {}", width, height);
    LIA::AppWindow::windowResized(width, height);
}
*/
void character_callback(GLFWwindow* window, unsigned int codepoint) {
    LIA_trace(std::vformat("Character pressed {}", std::make_format_args(codepoint)));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action) {
        case GLFW_PRESS:
            LIA_trace(std::vformat("Key pressed {}", std::make_format_args(key)));
            LIA::AppWindow::keyPressed(key);
        break;
        case GLFW_RELEASE:
            LIA_trace(std::vformat("Key released {}", std::make_format_args(key)));
            LIA::AppWindow::keyReleased(key);
        break;
        default:
        break;
    }
}

bool LIA::AppWindow::handleCheckbox(Event& event) {
    if (event.name.compare("checkbox_action") != 0) {
        return false;
    }
    if (event.source.compare("settings") == 0) {
        if (event.action.compare("fullscreen") == 0) {
            _glfwSettings.fullscreen = !_glfwSettings.fullscreen;
            LIA_trace_f("fullscreen changed to {}", _glfwSettings.fullscreen);
            if (_glfwSettings.fullscreen) {        
                glfwGetWindowPos(_glfwWindow , &windowLastX, &windowLastY);
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode * mode = glfwGetVideoMode(monitor);
                LIA_trace_f("Setting fullscreen {} x {}", mode->width, mode->height);
                glfwSetWindowMonitor(_glfwWindow, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
            } else {
                glfwSetWindowMonitor(_glfwWindow, NULL, windowLastX, windowLastY, _glfwSettings.width, _glfwSettings.height, GLFW_DONT_CARE);
            }
            UpdateGuiEvent updateGuiEvent("settings", "fullscreen", _glfwSettings.fullscreen);
            _eventManager->handleEvent(updateGuiEvent);
            return true;
        }
    }
    else {
        LIA_warn(std::vformat("Action {} not recognized from {}", std::make_format_args(event.action, event.source)));
    }
    return false;
}

#include "data/position.hpp"
bool LIA::AppWindow::handleGuiGetData(Event& event) {
    if (event.name.compare("get_data_gui") != 0) {
        return false;
    }
    if (event.source.compare("settings") == 0) {
        UpdateGuiEvent updateGuiEvent(event.window, "fullscreen", _glfwSettings.fullscreen);
        _eventManager->handleEvent(updateGuiEvent);
        std::string res = std::vformat("{} x {}", std::make_format_args(_glfwSettings.width, _glfwSettings.height));
        UpdateGuiEvent updateResolutionEvent("settings", "resolution", res);
        _eventManager->handleEvent(updateResolutionEvent);
        return true;
    }
    if (event.source.compare("debug") == 0) {
        Position& pos = _mainCamera.getPosition();
        std::string cPos = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(pos.x, pos.y, pos.z));
        UpdateGuiEvent updateGuiEvent(event.window, "camera_position", cPos);
        _eventManager->handleEvent(updateGuiEvent);
        Position& lookAt = _mainCamera.getLookAt();
        std::string cLookAt = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(lookAt.x, lookAt.y, lookAt.z));
        UpdateGuiEvent updateLookAtEvent(event.window, "camera_lookAt", cLookAt);
        _eventManager->handleEvent(updateLookAtEvent);
        return true;
    }
    return false;
}
/*
void LIA::AppWindow::onCameraPositionChanged() {
        Position& pos = _mainCamera.getPosition();
        std::string cPos = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(pos.x, pos.y, pos.z));
        UpdateGuiEvent updateGuiEvent("debug", "camera_position", cPos);
        _eventManager->handleEvent(updateGuiEvent);
        Position& lookAt = _mainCamera.getLookAt();
        std::string cLookAt = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(lookAt.x, lookAt.y, lookAt.z));
        UpdateGuiEvent updateLookAtEvent("debug", "camera_lookAt", cLookAt);
        _eventManager->handleEvent(updateLookAtEvent);
}
*/

bool LIA::AppWindow::registerHandlers(EventManager* eventManager) {
    eventManager->subscribe("checkbox_action", EventType::GUI, std::bind(&AppWindow::handleCheckbox, this, std::placeholders::_1));
    eventManager->subscribe("get_data_gui", EventType::GUI, std::bind(&AppWindow::handleGuiGetData, this, std::placeholders::_1));
    return true;
}

void LIA::AppWindow::loadSettings() {
    LIA_debug("Reading settings");

    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load("./data/settings/appWindow.xml");
        
    _glfwSettings.minor = xmlLoader.getInt(xmlData, "minor");
    _glfwSettings.major = xmlLoader.getInt(xmlData, "major");
    _glfwSettings.compact = xmlLoader.getBoolean(xmlData, "compact");
    _glfwSettings.samples = xmlLoader.getInt(xmlData, "samples");

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);
    LIA_trace_f("Setting resolution to {} x {}", mode->width, mode->height);

    _glfwSettings.width = xmlLoader.getInt(xmlData, "width", mode->width);
    _glfwSettings.height = xmlLoader.getInt(xmlData, "height", mode->height);

    _glfwSettings.experimental = xmlLoader.getBoolean(xmlData, "experimental");
    _glfwSettings.maximize = xmlLoader.getBoolean(xmlData, "maximize");
    _glfwSettings.vSync = xmlLoader.getBoolean(xmlData, "vSync", true);

    _glfwSettings.fullscreen = xmlLoader.getBoolean(xmlData, "fullscreen", false);

    std::map<std::string, XmlLoader::XmlNode> nodes = xmlData.nodes;
    XmlLoader::XmlNode backgroundNode = nodes.at("background");
    Color bgColor = xmlLoader.getColor(backgroundNode);
    _background.r = bgColor.r;
    _background.g = bgColor.g;
    _background.b = bgColor.b;
    _background.a = bgColor.a;
/*
    _cameraSettings.locked = xmlLoader.getBoolean(xmlData, "lockedCamera", false);
    _cameraSettings.ortho = xmlLoader.getBoolean(xmlData, "orthoCamera", false);
    _cameraSettings.position = xmlLoader.getPosition(xmlData, "cameraPosition");
    _cameraSettings.step = xmlLoader.getFloat(xmlData, "cameraStep", 0.1);
*/
/*
    XmlLoader::XmlData xmlCamera = xmlLoader.load("./data/settings/controls/camera.xml");
    _cameraControl.left = xmlLoader.getChar(xmlCamera, "left", '\0');
    _cameraControl.right = xmlLoader.getChar(xmlCamera, "right", '\0');
    _cameraControl.up = xmlLoader.getChar(xmlCamera, "up", '\0');
    _cameraControl.down = xmlLoader.getChar(xmlCamera, "down", '\0');
    _cameraControl.forward = xmlLoader.getChar(xmlCamera, "forward", '\0');
    _cameraControl.backward = xmlLoader.getChar(xmlCamera, "backward", '\0');
*/
}

bool LIA::AppWindow::init(std::string windowName) {
    _closeThis = false;
    _eventManager = &(LIA::Engine::getInstance().getEventManager());
    if (!_eventManager->isSet()) {
        LIA_fatal("EventManager has to be initialized before AppWindow");
        return false;
    }

    if (!_keyController.init()) {
        LIA_fatal("Could not initialise key controller");
        return false;
    }

    LIA_info(std::format("Init GLFW {}", glfwGetVersionString()));
    glfwSetErrorCallback( error_callback_glfw );

    if (!glfwInit()) {
       LIA_fatal("Could not start GLFW3.");
       return false;
    }
    
    loadSettings();

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _glfwSettings.major );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _glfwSettings.minor );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, _glfwSettings.compact );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_SAMPLES, _glfwSettings.samples );

    _glfwWindow = glfwCreateWindow( _glfwSettings.width, _glfwSettings.height, windowName.c_str(), NULL, NULL );
    if ( !_glfwWindow ) {
        LIA_fatal("Could not open window with GLFW3");
        glfwTerminate();
        return false;
    }

    LIA_debug("Setting callbacks");
    glfwSetCursorPosCallback(_glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(_glfwWindow, mouse_button_callback);
//    glfwSetCharCallback(_glfwWindow, character_callback);
    glfwSetKeyCallback(_glfwWindow, key_callback);
//    glfwSetWindowSizeCallback(_glfwWindow, window_size_callback);

    if (_glfwSettings.maximize) {
        LIA_debug("Maximizing window");
        glfwMaximizeWindow(_glfwWindow);
    }

    LIA_debug("Setting current context");
    glfwMakeContextCurrent( _glfwWindow );

    glewExperimental = _glfwSettings.experimental;
    if (glewInit() != GLEW_OK) 
    {
        LIA_fatal("Failed to initialize GLEW");
        return false;
    }

    if (!_glfwSettings.vSync) {
        LIA_info("Disabling vSync");
        glfwSwapInterval(0);
    }
    glfwGetWindowPos(_glfwWindow , &windowLastX, &windowLastY);
    if (_glfwSettings.fullscreen) {
        LIA_debug("Switching to fullscreen"); 
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        LIA_trace_f("Setting fullscreen {} x {}", mode->width, mode->height);
        glfwSetWindowMonitor(_glfwWindow, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    }

    LIA_trace(std::vformat("Renderer: {}\n", std::make_format_args(reinterpret_cast<const char*>(glGetString( GL_RENDERER )))));
    LIA_trace(std::vformat("OpenGL version supported {}\n",  std::make_format_args(reinterpret_cast<const char*>(glGetString( GL_VERSION)))));
    LIA_trace(std::vformat("GLSL VERSION: {}\n",  std::make_format_args(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)))));

    if (!_scene.init()) {
        LIA_fatal("Failed to init scene");
        return false;
    }
    if (!_gui.init()) {
        LIA_fatal("Failed to init gui");
        return false;
    }
    if (!_terrain.init()) {
        LIA_fatal("Failed to init terrain scene");
        return false;
    }
    if (!_mainCamera.init()) {
        LIA_fatal("Failed to init main camera");
        return false;
    }
    if (!_mainCamera.loadFromSettings("./data/settings/camera.xml")) {
        LIA_fatal("Failed to load main camera settings");
        return false;
    }
    /*
    if (_cameraSettings.ortho) {
        _mainCamera.switchOrtho();
    }
    _mainCamera.setLocked(_cameraSettings.locked);
    _mainCamera.setPosition(_cameraSettings.position);
    */
    if (!_guiCamera.init()) {
        LIA_fatal("Failed to init gui camera");
        return false;
    }
    _guiCamera.switchOrtho();
    if (!registerHandlers(_eventManager)) {
        LIA_fatal("Failed to register handlers");
        return false;
    }
    return true;
}

bool LIA::AppWindow::initPrefab() {
    ShaderManager& shaderManager = LIA::Engine::getInstance().getShaderManager();
    if (!_scene.initPrefab(shaderManager)) {
        LIA_fatal("Failed to init scene prefabs");
        return false;
    }
    if (!_gui.initPrefab(shaderManager)) {
        LIA_fatal("Failed to init gui prefabs");
        return false;
    }
}

bool LIA::AppWindow::initFont(/*GLuint programId*/) {
    ShaderManager* shaderManager = &(LIA::Engine::getInstance().getShaderManager());
    if (!_font.initialise(/*programId*/shaderManager)) {
        LIA_fatal("Failed to init font");
        return false;
    }
    
    _font.resizeScreen(windowW, windowH);
    return true;
}

void LIA::AppWindow::handleMouseInput() {
    // Process button events
    if (getButtonEvent() == 1) {
        int x, y;
        glfwGetWindowPos(_glfwWindow , &x, &y);
        glfwSetWindowPos(_glfwWindow , x + _offsetCpX, y + _offsetCpY);
        resetOffset();
    }
}

void LIA::AppWindow::update() {
    glfwPollEvents();
    handleMouseInput();

    _mainCamera.update(_eventManager);
}

void LIA::AppWindow::draw() {
    ShaderManager* shaderManager = &(LIA::Engine::getInstance().getShaderManager());
    // Handle Window resize
    glfwGetWindowSize( _glfwWindow, &windowW, &windowH );
    if (windowH != windowHold || windowW != windowWold) {
        LIA_trace(std::vformat("Window resized to {} x {}", std::make_format_args(windowW, windowH)));
        windowWold = windowW;
        windowHold = windowH;
        glViewport(0, 0, windowW, windowH);
        
        _mainCamera.update(windowW, windowH);
        _guiCamera.update(windowW, windowH);
        _font.resizeScreen(windowW, windowH);
    }
    // Draw
    if (windowW > 0 && windowH > 0) {
        glClearColor(_background.r, _background.g, _background.b, _background.a);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS); 

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!_terrain.draw(_mainCamera, shaderManager)) {
            LIA_fatal("Failed to draw terrain");
            _closeThis = true;
            return;
        }
        if (!_scene.draw(_mainCamera, shaderManager)) {
            LIA_fatal("Failed to draw scene");
            _closeThis = true;
            return;
        }
    //    glDisable(GL_DEPTH_TEST);
        if (!_gui.draw(_guiCamera, shaderManager)) {
            LIA_fatal("Failed to draw gui");
            _closeThis = true;
            return;
        }
    //    glEnable(GL_DEPTH_TEST);
        _font.draw(_guiCamera, shaderManager);
    }

    // Swap buffers
    glfwSwapBuffers( _glfwWindow );
}

LIA::AppWindow::~AppWindow() {
    LIA_trace("Destroying window");
    LIA_trace("Terminating glfw");
    glfwTerminate();
}

void LIA::AppWindow::close() {
    _closeThis = true;
}

bool LIA::AppWindow::isExit() {
    return _closeThis || glfwWindowShouldClose( _glfwWindow );
}

bool LIA::AppWindow::isKeyPressedPoll(int key) {
    return glfwGetKey(_glfwWindow, key) == GLFW_PRESS;
}

bool LIA::AppWindow::isKeyReleasedPoll(int key) {
    return glfwGetKey(_glfwWindow, key) == GLFW_RELEASE;
}

bool LIA::AppWindow::isCursorInsideWindow() {
    return glfwGetWindowAttrib(_glfwWindow, GLFW_HOVERED);
}

LIA::Position LIA::AppWindow::getMousePos() {
    double x, y;
    glfwGetCursorPos(_glfwWindow, &x, &y);
    Position pos;
    pos.x = x;
    pos.y = y;
    return pos;
}

LIA::Scale LIA::AppWindow::getWindowScale() {
    int x, y;
    glfwGetWindowSize( _glfwWindow, &x, &y );
    Scale scale;
    scale.x = x;
    scale.y = y;
    return scale;
}