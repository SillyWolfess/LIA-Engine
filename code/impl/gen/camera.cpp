#include "camera.hpp"
#include "precompiled.hpp"

#include "data/Event.hpp"
//TODO remove
#include "appWindow.hpp"
#include "manager/eventManager.hpp"
#include "Engine.hpp"

bool LIA::Camera::init() {
    LIA_info("Camera init");
    _perspSettings.fov = 45.0f;
    _perspSettings.near = 0.1f;
    _perspSettings.far = 500.0f;
    _perspSettings.res = 1.0f;

    _orthoSettings.near = -1.0f;
    _orthoSettings.far = 10.0f;
    _orthoSettings.h = 800;
    _orthoSettings.w = 800;

    _ortho = false;

    int x = 0;
    int y = 0;
    _position.x = 0 + x;
    _position.y = 0 + y;
    _position.z = -5;

    _worldDirection.x = 0;
    _worldDirection.y = 1;
    _worldDirection.z = 0;

    _lookAt.x = 0;// + x;
    _lookAt.y = 0;// + y;
    _lookAt.z = 0;
    return true;
}

void LIA::Camera::update(EventManager *eventManager) {
    if (isLocked()) {
        return;
    }
    if (_settings.controls != "" && _watcher.needsReload(_settings.controls)) {
        if (!loadControls(_settings.controls)) {
            LIA_error("Failed to reload camera controls");
        }
    }
    if (AppWindow::isKeyPressed("camera", _cameraControls.get("up"))) {
        Position pos = emptyPosition();
        pos.y = _settings.step;
        move(pos);
        onPositionChanged(eventManager);
    } else if (AppWindow::isKeyPressed("camera", _cameraControls.get("down"))) {
        Position pos = emptyPosition();
        pos.y = -_settings.step;
        move(pos);
        onPositionChanged(eventManager);
    } else  if (AppWindow::isKeyPressed("camera", _cameraControls.get("right"))) {
        Position pos = emptyPosition();
        pos.x = -_settings.step;
        move(pos);
        onPositionChanged(eventManager);
    } else if (AppWindow::isKeyPressed("camera", _cameraControls.get("left"))) {
        Position pos = emptyPosition();
        pos.x = _settings.step;
        move(pos);
        onPositionChanged(eventManager);
    } else  if (AppWindow::isKeyPressed("camera", _cameraControls.get("forward"))) {
        Position pos = emptyPosition();
        pos.z = _settings.step;
        move(pos);
        onPositionChanged(eventManager);
    } else if (AppWindow::isKeyPressed("camera", _cameraControls.get("backward"))) {
        Position pos = emptyPosition();
        pos.z = -_settings.step;
        move(pos);
        onPositionChanged(eventManager);
    }
}

void LIA::Camera::onPositionChanged(EventManager *eventManager) {
        Position& pos = getPosition();
        std::string cPos = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(pos.x, pos.y, pos.z));
        UpdateGuiEvent updateGuiEvent("debug", "camera_position", cPos);
        eventManager->handleEvent(updateGuiEvent);
        Position& lookAt = getLookAt();
        std::string cLookAt = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(lookAt.x, lookAt.y, lookAt.z));
        UpdateGuiEvent updateLookAtEvent("debug", "camera_lookAt", cLookAt);
        eventManager->handleEvent(updateLookAtEvent);
}

void LIA::Camera::update(float w, float h) {
    if (h > 0) {
        _perspSettings.res = w / h;
    }
    _orthoSettings.w = w;
    _orthoSettings.h = h;
}

LIA::Camera::~Camera() {
    LIA_trace("Camera destroyed");
}

glm::mat4 LIA::Camera::getProjection() {
    if (_ortho) {
        return glm::ortho(0.0f, _orthoSettings.w, _orthoSettings.h, 0.0f, _orthoSettings.near, _orthoSettings.far);
    }
    return glm::perspective(
        glm::radians(_perspSettings.fov),
        _perspSettings.res,
        _perspSettings.near,
        _perspSettings.far
    );
}

glm::mat4 LIA::Camera::getView() {
    if (_ortho) {
        return glm::mat4(1.0f);
    }
    return glm::lookAt(
        glm::vec3(_position.x, _position.y, _position.z),
        glm::vec3(_lookAt.x, _lookAt.y, _lookAt.z),
        glm::vec3(_worldDirection.x, _worldDirection.y, _worldDirection.z)
    );
}

void LIA::Camera::move(Position& position) {
    _position.x = _position.x + position.x;
    _position.y = _position.y + position.y;
    _position.z = _position.z + position.z;

    _lookAt.x =  _lookAt.x + position.x;
    _lookAt.y =  _lookAt.y + position.y;
    _lookAt.z =  _lookAt.z + position.z;
}

void LIA::Camera::switchOrtho() {
    _ortho = true;
}

void LIA::Camera::swtichPersp() {
    _ortho = false;
}

bool LIA::Camera::loadControls(std::string path) {
    if (path == "") {
        return true;
    }
    LIA_TRY
        KeybindingManager& keybindingManager = Engine::getInstance().getKeybindingManager();
        if (!keybindingManager.registerControls("camera", path)) {
            LIA_error_f("Failed to register keybindings for camera from '{}'", path);
            return false;
        }
        if (!keybindingManager.load("camera")) {
            LIA_error_f("Failed to load keybindings for camera");
            return false;
        }
        _cameraControls = keybindingManager.getControls("camera");
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::Camera::loadFromSettings(std::string path) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlData = xmlLoader.load(path);
        _settings.locked = xmlLoader.getBoolean(xmlData, "locked", false);
        _settings.ortho = xmlLoader.getBoolean(xmlData, "ortho", false);
        _settings.position = xmlLoader.getPosition(xmlData, "position");
        _settings.step = xmlLoader.getFloat(xmlData, "step", 0.1);
        _settings.controls = xmlLoader.getString(xmlData, "controls", "");

        if (_settings.ortho) {
            switchOrtho();
        }
        setLocked(_settings.locked);
        setPosition(_settings.position);

        if (_settings.controls != "") {
            _watcher.subscribe(_settings.controls);
        }
        if (!loadControls(_settings.controls)) {
            LIA_error("Failed to load camera controls");
            return false;
        }
        return true;
    LIA_CATCH_RETURN_FALSE
}