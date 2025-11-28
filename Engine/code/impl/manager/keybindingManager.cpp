#include "manager/keybindingManager.hpp"
#include "tools/macros.hpp"
#include "logs.hpp"
#include "loaders/xmlLoader.hpp"

void LIA::KeybindingControls::set(std::string name, int code) {
    _map[name] = code;
}

int LIA::KeybindingControls::get(std::string name) {
    LIA_TRY
        return _map[name];
    LIA_CATCH(return 0;)
}

bool LIA::KeybindingManager::init() {
    LIA_info("Keybinding manager initialized");
    return true;
}

LIA::KeybindingManager::KeybindingManager() {
    LIA_trace("KeybindingManager constructor");
}

LIA::KeybindingManager::~KeybindingManager() {
    LIA_trace("KeybindingManager destroyed");
}

bool LIA::KeybindingManager::registerControls(std::string name, std::string path) {
    LIA_TRY
        _pathMap[name] = path;
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::KeybindingManager::registerControls(std::string name, std::string path, bool watch) {
    LIA_TRY
        _pathMap[name] = path;
        if (watch) {
            _watcher.subscribe(path);
        }
        return true;
    LIA_CATCH_RETURN_FALSE
}

LIA::KeybindingControls& LIA::KeybindingManager::getControls(std::string name) {    
    return _controls[name];
}

bool LIA::KeybindingManager::load(std::string name) {
    LIA_TRY
        std::string path = _pathMap[name];
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlData = xmlLoader.load(path);
        if (_controls.find(name) == _controls.end()) {
            _controls.emplace(std::pair<std::string, KeybindingControls>(name, KeybindingControls()));
        }
        KeybindingControls& controls = _controls[name];
        controls.clear();
        for (auto [key, value]: xmlData.values) {
            XmlLoader::XmlNode node = xmlLoader.getNode(xmlData, key);
            int keyCode = 0;
            keyCode = value[0];
            controls.set(key, keyCode);
            LIA_trace_f("Key mapping '{}' = '{}'", key, keyCode);
        }

        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::KeybindingManager::update() {
    LIA_TRY

        return true;
    LIA_CATCH_RETURN_FALSE
}

std::vector<std::string> LIA::KeybindingControls::keySet() {
    std::vector<std::string> list;
    list.reserve(_map.size());
    std::map<std::string, int>::iterator it;
    for (it = _map.begin(); it != _map.end(); ++it) {
        list.push_back(it->first);
    }
    return list;
}