#include "manager/animationManager.hpp"

#include "logs.hpp"
#include "precompiled.hpp"

void LIA::AnimationManager::reset() {
    _animDefinitions.clear();
}

int LIA::AnimationManager::create() {
    LIA_debug("Creating animation object");
    _animDefinitions.emplace_back();
    int indx = _animDefinitions.size();
    _animDefinitions[indx].loaded = false;
    LIA_trace_f("Created animaiton object. Current size {}", _animDefinitions.size());
    return indx;
}

LIA::AnimationDefinition* LIA::AnimationManager::get(int indx) {
    return &_animDefinitions.at(indx);
}

int LIA::AnimationManager::create(std::string animationName) {
    int id = -1;
    LIA_TRY
        LIA_debug_f("Creating animation definition {}", animationName);
        std::string animDefPath = getTemplatePath(animationName);
        XmlLoader xmlLoader;
        if (animDefPath.compare("") == 0) {
            // Get data from xml
            XmlLoader::XmlData xmlData;
            if (_pathCache.contains("./data/animations/list.xml")) {
                LIA_trace("Reading ./data/animations/list.xml from cache");
                xmlData = _pathCache.at("./data/animations/list.xml");
            } else {
                LIA_trace("Reading ./data/animations/list.xml from filesystem");
                xmlData = xmlLoader.load("./data/animations/list.xml");
                _pathCache.emplace("./data/animations/list.xml", xmlData);
            }
            if (xmlData.values.find(animationName) == xmlData.values.end()) {
                LIA_error_f("Cannot find animation definition for '{}'", animationName);
                return -1;
            }
            animDefPath = xmlData.values.at(animationName);
        }

        XmlLoader::XmlData xmlAnimationData;
        if (_animDefCache.contains(animDefPath)) {
            LIA_trace_f("Reading {} with path {} from cache", animationName, animDefPath);
            xmlAnimationData = _animDefCache.at(animDefPath);
        } else {
            LIA_trace_f("Reading {} with path {} from filesystem", animationName, animDefPath);
            xmlAnimationData = xmlLoader.load(animDefPath);
            _animDefCache.emplace(animDefPath, xmlAnimationData);
        }

        std::string name = xmlLoader.getString(xmlAnimationData, "name");
        std::string path = xmlLoader.getString(xmlAnimationData, "path");
        std::string type = xmlLoader.getString(xmlAnimationData, "type");
        bool repeat = xmlLoader.getBoolean(xmlAnimationData, "repeat", false);

        id = create();
        if (id < 0) {
            LIA_error_f("Failed to create animation object for '{}'", animationName);
            return -1;
        }

        AnimationDefinition* data = get(id);
        data->name = name;
        data->path = path;
        data->type = resolveType(type);
        data->repeat = repeat;
        LIA_debug_f("Created animation object {} with id {}", animationName, id);
    LIA_CATCH(return -1;)
    return id;
}

bool LIA::AnimationManager::registerPath(std::string name, std::string path) {
    LIA_trace_f("Adding path '{}' for '{}'", path, name);
    _paths.emplace(std::pair<std::string, std::string>(name, path));
    return true;
}

std::string LIA::AnimationManager::getPath(std::string name) {
    LIA_TRY
        return _paths[name];
    LIA_CATCH(return "";)
}

bool LIA::AnimationManager::registerTemplatePath(std::string name, std::string path) {
    LIA_trace_f("Adding template path '{}' for '{}'", path, name);
    _templatePaths.emplace(std::pair<std::string, std::string>(name, path));
    return true;
}

std::string LIA::AnimationManager::getTemplatePath(std::string name) {
    LIA_TRY
        return _templatePaths[name];
    LIA_CATCH(return "";)
}

bool LIA::AnimationManager::registerPathsFromFile(std::string file) {
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load(file);
    for (auto [name, path] : xmlData.values) {
        if (!registerPath(name, path)) {
            LIA_error_f("Failed to register path {} for {}", path, name);
            return false;
        }
    }
    return true;
}

bool LIA::AnimationManager::registerTemplatePathsFromFile(std::string file) {
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load(file);
    for (auto [name, path] : xmlData.values) {
        if (!registerTemplatePath(name, path)) {
            LIA_error_f("Failed to register template path {} for {}", path, name);
            return false;
        }
    }
    return true;
}

LIA::AnimationType LIA::AnimationManager::resolveType(std::string type) {
    if (type.compare("frames") == 0) {
        return AnimationType::FRAMES;
    }
    return AnimationType::NONE;
}

