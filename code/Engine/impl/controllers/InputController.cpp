#include "controllers/InputController.hpp"
#include "precompiled.hpp"

bool LIA::KeyController::init() {
    for (int i = 0; i < 500; i++) {
        _keyNames[i] = std::vformat("{}", std::make_format_args(i));
    }
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlData = xmlLoader.load("./data/xml/settings/keyInput.xml");
        for (auto [code, name] : xmlData.values) {
            int keyCode = xmlLoader.integer(code);
            if (isKeyCodeValid(keyCode)) {
                _keyNames[keyCode] = name;
            }
        }
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::KeyController::isKeyCodeValid(int keyCode) {
    return keyCode > 0 && keyCode <= 500;
}

bool LIA::KeyController::isKeyPressed(std::string keyGroup, int keyCode) {
    if (!isKeyCodeValid(keyCode)) {
        return false;
    }
    return _guiKeys[keyCode];
}

void LIA::KeyController::setKeyPressed(int keyCode) {
    if (!isKeyCodeValid(keyCode)) {
        return;
    }
    _guiKeys[keyCode] = true;
    _lastKey = keyCode;
}

void LIA::KeyController::setKeyReleased(int keyCode) {
    if (!isKeyCodeValid(keyCode)) {
        return;
    }
    _guiKeys[keyCode] = false;
}

int LIA::KeyController::getLastKeyPressed() {
    return _lastKey;
}

std::string LIA::KeyController::getKeyName(int keyCode) {
    if ('a' <= keyCode && keyCode <= 'z') {
        return std::string{(char) keyCode};
    }
    if ('A' <= keyCode && keyCode <= 'Z') {
        return std::string{(char) keyCode};
    }
    if ('0' <= keyCode && keyCode <= '9') {
        return std::string{(char) keyCode};
    }
    return _keyNames[keyCode];
}