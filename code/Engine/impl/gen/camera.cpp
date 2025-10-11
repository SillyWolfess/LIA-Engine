#include "camera.hpp"
#include "precompiled.hpp"

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