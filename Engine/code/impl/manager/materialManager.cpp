#include "manager/materialManager.hpp"
#include "tools/macros.hpp"
#include "logs.hpp"

bool LIA::MaterialManager::init() {
    return true;
}

bool LIA::MaterialManager::load() {
    return true;
}

LIA::Material& LIA::MaterialManager::get(std::string name) {
    return _materials[name];
}

bool LIA::MaterialManager::load(std::string name) {
    LIA_TRY
        Material& material = get(name);
        if (material.isLoaded) {
            LIA_trace_f("[FOUND_VALID] {}", material.name);
            return true;
        }
        LIA_trace_f("Loading material {}", material.name);
        if (!MaterialLoader::load(material)) {
            LIA_error_f("Failed to load material {}", material.name);
            return false;
        }
        LIA_trace_f("New material {} loaded", name);
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::MaterialManager::registerMaterial(std::string name, std::string folder, std::string path) {
    if (_materials.find(name) == _materials.end()) {
        Material tmp;
        _materials.emplace(std::pair<std::string, Material>(name, tmp));
        _materialNames.emplace_back(name);
    } else {
        LIA_warn_f("Material '{}' is already registered", name);
        return true;
    }
    Material& material = get(name);
    material.name = name;
    material.folder = folder;
    material.path = path;
    material.isLoaded = false;
    LIA_trace_f("Registered material '{}' with path '{}' in folder '{}'", name, path, folder);
    return true;
}