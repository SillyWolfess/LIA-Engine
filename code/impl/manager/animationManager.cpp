#include "manager/animationManager.hpp"

#include "logs.hpp"
#include "precompiled.hpp"
#include "graphics/scene.hpp"
#include "data/Object.hpp"

void LIA::AnimationManager::reset() {
    _animDefinitions.clear();
}

bool LIA::AnimationManager::_create(std::string name) {
    LIA_TRY
        LIA_debug("Creating animation object");
        AnimationDefinition tmp;
        _animDefinitions.emplace(std::pair<std::string, AnimationDefinition>(name, tmp));
        _animDefinitions[name].loaded = false;
        _animDefinitions[name].name = name;
        LIA_trace_f("Created animaiton object {}. Current size {}", _animDefinitions[name].name, _animDefinitions.size());
        return true;
    LIA_CATCH_RETURN_FALSE
}

LIA::AnimationDefinition* LIA::AnimationManager::get(std::string name) {
    return &_animDefinitions.at(name);
}

bool LIA::AnimationManager::find(std::string name) {
    return _animDefinitions.find(name) != _animDefinitions.end();
}

bool LIA::AnimationManager::create(std::string animationName) {
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

        std::string name = xmlLoader.getString(xmlAnimationData, "name", animationName);
        std::string path = xmlLoader.getString(xmlAnimationData, "path");
        std::string type = xmlLoader.getString(xmlAnimationData, "type");
        bool repeat = xmlLoader.getBoolean(xmlAnimationData, "repeat", false);

        if (!create(name)) {
            LIA_error_f("Failed to create animation object for '{}'", animationName);
            return false;
        }

        AnimationDefinition* data = get(name);
        data->name = name;
        data->path = path;
        data->type = resolveType(type);
        data->repeat = repeat;
        LIA_debug_f("Created animation object {}", animationName);
        return true;
    LIA_CATCH_RETURN_FALSE
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

void LIA::AnimationManager::pass(Scene* scene, Object* object, AnimationInfo& animInfo, Position& position, Rotation& rotation, Scale& scale) {
    if (!animInfo.playing) {
        LIA_warn_f("Trying to pass not playing animation {}", animInfo.name);
        return;
    }
    if (!find(animInfo.name)) {
        LIA_warn_f("Trying to pass not existing animation {}", animInfo.name);
        return;
    }
    AnimationDefinition* def = get(animInfo.name);
    if (!def->loaded) {
        LIA_warn_f("Animation {} is not loaded", def->name);
        return;
    }
    if (def->type == AnimationType::FRAMES) {
        _frameAnimationManager.pass(scene, object, animInfo.frame, def->path, position, rotation, scale);
    } else {
        LIA_error_f("Unsupported animation type {} for {}", def->type, def->name);
    }
}

void LIA::AnimationManager::nextFrame(AnimationInfo& animInfo) {
    if (!animInfo.playing) {
        LIA_warn_f("Next Frame called while not playing animation {}", animInfo.name);
        return;
    }
    if (!find(animInfo.name)) {
        LIA_warn_f("Trying to pass not existing animation {}", animInfo.name);
        return;
    }
    AnimationDefinition* def = get(animInfo.name);
    if (!def->loaded) {
        LIA_warn_f("Animation {} is not loaded", def->name);
        return;
    }
    if (def->type == AnimationType::FRAMES) {
        // At the end of the loop
        if (_frameAnimationManager.nextFrame(def->name, animInfo)) {
            if (def->repeat) {
                _frameAnimationManager.reset(def->name, animInfo);
            } else {
                animInfo.playing = false;
            }
        }
    }
    else {
        LIA_error_f("Unsupported animation type {} for {}", def->type, def->name);
    }
}