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

LIA::Material& LIA::MaterialManager::getByLib(std::string libName, std::string name) {
    return _materialLibs[libName]._materials[name];
}

bool LIA::MaterialManager::load(std::string name) {
    LIA_TRY
        Material& material = get(name);
        if (material.isLoaded) {
            LIA_trace_f("[FOUND_VALID] {}", material.name);
            return true;
        }
        LIA_trace_f("Loading material {}", material.name);
        if (!MaterialLoader::loadFromLib(material)) {
            LIA_error_f("Failed to load material {}", material.name);
            return false;
        }
        LIA_trace_f("New material {} loaded", name);
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::MaterialManager::loadLib(std::string name) {
    LIA_TRY
        MaterialLib& lib = _materialLibs[name];
        if (lib._loaded) {
            LIA_trace_f("[FOUND VALID] {}", lib._name);
            return true;
        }
        LIA_trace_f("Loading material lib {}", lib._name);
        if (!MaterialLoader::loadLib(lib)) {
            LIA_error_f("Failed to load material lib {}", lib._name);
            return false;
        }
        LIA_trace_f("New material lib {} loaded with {} materials", name, lib._materials.size());
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

bool LIA::MaterialManager::registerMaterialLib(std::string name, std::string folder) {
    if (_materialLibs.find(name) == _materialLibs.end()) {
        MaterialLib tmp;
        _materialLibs.emplace(std::pair<std::string, MaterialLib>(name, tmp));
        _materialLibNames.emplace_back(name);
    } else {
        LIA_warn_f("Material lib '{}' is already registered", name);
        return true;
    }
    MaterialLib &lib = _materialLibs[name];
    lib._name = name;
    lib._folder = folder;
    lib._materials.clear();
    lib._loaded = false;
    LIA_trace_f("Registered material lib '{}' with folder '{}'", name, folder);
    return true;
}