#include "manager/objectManager.hpp"
#include "manager/shaderManager.hpp"
#include "graphics/scene.hpp"

#include "logs.hpp"

int LIA::ObjectManager::create() {
    LIA_debug("Creating object");
    _objects.emplace_back();
    int indx = _objects.size() - 1;
    _objects[indx]._name = "x";
    _objects[indx]._type = ObjectType::OBJECT;
    _objects[indx]._movement._speed = emptySpeed();
    LIA_trace(std::vformat("Created new object. Current size {}", std::make_format_args(_objects.size())));
    return indx;
}

int LIA::ObjectManager::create(std::string objectName) {
    int objectId = -1;
    LIA_TRY
        LIA_debug(std::vformat("Creating model from definition {}", std::make_format_args(objectName)));
        std::string objectPathDef = getTemplatePath(objectName);
        XmlLoader xmlLoader;
        if (objectPathDef.compare("") == 0) {
            // Get data from xml
            XmlLoader::XmlData xmlData = xmlLoader.load("./data/objects/list.xml");
            objectPathDef = xmlData.values.at(objectName);
        }
        XmlLoader::XmlData xmlObjectData = xmlLoader.load(objectPathDef);
        std::string mModel = xmlLoader.getString(xmlObjectData, "model");
        std::string oName = xmlLoader.getString(xmlObjectData, "name");

        objectId = create();
        Object* object = get(objectId);
        object->_name = oName;
        setModel(object, mModel);
        LIA_debug_f("Created object {} with id = {}", objectName, objectId);
    LIA_CATCH(return -1;)
    return objectId;
}

LIA::Object* LIA::ObjectManager::get(int indx) {
    return &_objects.at(indx);
}

int LIA::ObjectManager::size() {
    return _objects.size();
}

int LIA::ObjectManager::getPlayer(int player) {
    if (-1 < player && player < _objects.size()) {
        if (_objects[player]._type == ObjectType::PLAYER) {
            return player;
        }
    }
    for (int i = 0; i < _objects.size(); i++) {
        Object& object = _objects[i];
        if (object._type == ObjectType::PLAYER) {
            return i;
        }
    }
    return -1;
}

int LIA::ObjectManager::getByName(int id, std::string name) {
    if (-1 < id && id < _objects.size()) {
        if (_objects[id]._name.compare(name) == 0) {
            return id;
        }
    }
    for (int i = 0; i < _objects.size(); i++) {
        Object& object = _objects[i];
        if (object._name.compare(name) == 0) {
            return i;
        }
    }
    return -1;
}

void LIA::ObjectManager::reset() {
    _objects.clear();
}

bool LIA::ObjectManager::load(ShaderManager* shaderManager) {
    LIA_info("Loading objects");
    if (!_modelManager.load(shaderManager)) {
        LIA_error("Failed to load models");
        return false;
    }
    for (Object &object: _objects) {
        Model* model = _modelManager.get(object._modelInfo.id);
        ModelInfo* info = &object._modelInfo;
        info->hasIndices = model->hasIndices;
        info->loaded = model->isInGpu;
        info->size = model->size;
        info->vao = model->vao;
        info->shader = model->shader;

        if (object._modelInfo.size == 0) {
            LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
            return false;
        } else if (!object._modelInfo.loaded) {
            LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
            return false;
        } else {
            LIA_trace(std::vformat("Object {} loaded", std::make_format_args(object._name)));
        }
    }
    _modelManager.logNameToModelMap();
    _modelManager.logModels();
    LIA_debug(std::vformat("Objects.size: {}, Models.size: {}", std::make_format_args(_objects.size(), _modelManager.size())));
    return true;
}

void LIA::ObjectManager::pass(Scene *scene) {
    for (Object &object: _objects) {
        if (object._hide) {
            continue;
        }
        if (object._modelInfo.loaded) {            
            Model* model = _modelManager.get(object._modelInfo.id);
            std::vector<int> offsets = model->data.offsets;
            for (int i = 0; i < offsets.size(); i++) {
                int oSize = object._modelInfo.size - offsets[i];
                if (i + 1 < offsets.size()) {
                    oSize = offsets[i + 1] - offsets[i];
                }
                scene->add(
                    object._name,
                    object._position, object._rotation, object._scale,
                    object._modelInfo.vao, object._modelInfo.shader,
                    object._modelInfo.hasIndices, oSize,
                    model->data.materials, model->data.materialIds,
                    model->_textures, model->_bump, model->_em,
                    offsets[i]
                );
            }
        } else {
            LIA_error(std::vformat("Object {} was not loaded", std::make_format_args(object._name)));
        }
    }    
}

#include <iostream>
void LIA::ObjectManager::print(Object* object) {
    LIA_debug(std::vformat("{}", std::make_format_args(object->_name)));
}

void LIA::ObjectManager::print() {
    LIA_debug("Printing objects:");
    for (Object &object: _objects) {
        print(&object);
    }
}
/*
void LIA::ObjectManager::setModel(Object* object, std::string folder, std::string path) {
    LIA_debug(std::vformat("Setting model {} to object {}", std::make_format_args(path, object->_name)));
    Model* model = _modelManager.create(folder, path);
    object->_modelInfo.id = model->indx;
    LIA_debug("Done");
}
*/
void LIA::ObjectManager::setModel(Object* object, std::string modelName) {
    LIA_TRY
        LIA_debug(std::vformat("Setting model {} to object {}", std::make_format_args(modelName, object->_name)));
        Model* model = _modelManager.create(modelName);
        object->_modelInfo.name = modelName;
        object->_modelInfo.id = model->indx;
        object->_modelInfo.infoLoaded = model->infoLoaded;
        LIA_debug("Done");
    LIA_CATCH_EMPTY
}

void LIA::ObjectManager::setPosition(Object *object, float x, float y, float z) {
    object->_position.x = x;
    object->_position.y = y;
    object->_position.z = z;
}

void LIA::ObjectManager::setRotation(Object *object, float x, float y, float z) {
    object->_rotation.x = x;
    object->_rotation.y = y;
    object->_rotation.z = z;
}

void LIA::ObjectManager::setScale(Object *object, float x, float y, float z) {
    object->_scale.x = x;
    object->_scale.y = y;
    object->_scale.z = z;
}

void LIA::ObjectManager::setScale(Object *object, float scale) {
    setScale(object, scale, scale, scale);
}

void LIA::ObjectManager::setRotation(Object *object, float rotation) {
    setRotation(object, rotation, rotation, rotation);
}

void LIA::ObjectManager::setPosition(Object* object, Position& position) {
    setPosition(object, position.x, position.y, position.z);
}

void LIA::ObjectManager::setType(Object* object, std::string type) {
    if (type.compare("player") == 0)  {
        object->_type = ObjectType::PLAYER;
    } else if (type.compare("object") == 0) {
        object->_type = ObjectType::OBJECT;
    } else {
        LIA_warn(std::vformat("Unknown type {} for object {}", std::make_format_args(type, object->_name)));
    }
} 

void LIA::ObjectManager::setRotation(Object* object, Rotation& rotation) {
    setRotation(object, rotation.x, rotation.y, rotation.z);
}

void LIA::ObjectManager::setScale(Object* object, Scale& scale) {
    setScale(object, scale.x, scale.y, scale.z);
}

bool LIA::ObjectManager::createObject(std::string id, std::string name) {
    std::string path = getPath(id);
    if (path.compare("") == 0) {
        LIA_error("Failed to get path for '{}'", id);
        return false;
    }
    return loadObject(path, name);
}

bool LIA::ObjectManager::loadObject(std::string path, std::string customName) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlObject = xmlLoader.load(path);
        LIA_trace("Reading object name");
        std::string name = xmlLoader.getValue(xmlObject, "object");
        LIA_trace("Reading object type");
        std::string type = xmlLoader.getString(xmlObject, "type");
        LIA_trace("Creating object");
        int indx = create(name);
        if (indx == -1) {
            return false;
        }
        Object* object = get(indx);
        if (customName.compare("") != 0) {
            object->_name = customName;
        }
        if (!object->_modelInfo.infoLoaded) {
            LIA_fatal_f("Failed to get info for the model of object {}", name);
            return false;
        }
        LIA_trace("Setting object type");
        setType(object, type);
        LIA_trace("Setting posititon");
        Position position = xmlLoader.getPosition(xmlObject);
        setPosition(object, position);
        LIA_trace("Setting scale");
        Scale scale = xmlLoader.getScale(xmlObject);
        setScale(object, scale);
        LIA_trace("Setting rotation");
        Rotation rotation = xmlLoader.getRotation(xmlObject);
        rotation = convertRotation(rotation);
        setRotation(object, rotation);
        
        LIA_trace("Getting physics node");
        XmlLoader::XmlNode physicsNode = xmlLoader.getNode(xmlObject, "physics");
        object->_collison._grounded = xmlLoader.getBoolean(physicsNode, "grounded", "true");
        object->_physics._mass = xmlLoader.getFloat(physicsNode, "mass", 0.0f);

        if (!loadModel(*object)) {
            LIA_error("Failed to load model");
            return false;
        }
    LIA_CATCH_RETURN_FALSE
    return true;
}


bool LIA::ObjectManager::loadModel(Object& object) {
    LIA_info("Loading objects");
    Model* model = _modelManager.get(object._modelInfo.id);
    if (!model->isInGpu) {
        if (!_modelManager.loadModel(object._modelInfo.id)) {
            return false;
        }
        model = _modelManager.get(object._modelInfo.id);
    }
    ModelInfo* info = &object._modelInfo;
    info->hasIndices = model->hasIndices;
    info->loaded = model->isInGpu;
    info->size = model->size;
    info->vao = model->vao;
    info->shader = model->shader;

    if (object._modelInfo.size == 0) {
        LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
        return false;
    } else if (!object._modelInfo.loaded) {
        LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
        return false;
    } else {
        LIA_trace(std::vformat("Object {} loaded", std::make_format_args(object._name)));
    }
    _modelManager.logNameToModelMap();
    _modelManager.logModels();
    LIA_debug(std::vformat("Objects.size: {}, Models.size: {}", std::make_format_args(_objects.size(), _modelManager.size())));
    return true;
}

bool LIA::ObjectManager::registerPath(std::string name, std::string path) {
    LIA_trace_f("Adding path '{}' for '{}'", path, name);
    _paths.emplace(std::pair<std::string, std::string>(name, path));
    return true;
}

std::string LIA::ObjectManager::getPath(std::string name) {
    LIA_TRY
        return _paths[name];
    LIA_CATCH(return "";)
}

bool LIA::ObjectManager::registerTemplatePath(std::string name, std::string path) {
    LIA_trace_f("Adding template path '{}' for '{}'", path, name);
    _templatePaths.emplace(std::pair<std::string, std::string>(name, path));
    return true;
}

std::string LIA::ObjectManager::getTemplatePath(std::string name) {
    LIA_TRY
        return _templatePaths[name];
    LIA_CATCH(return "";)
}

bool LIA::ObjectManager::registerPathsFromFile(std::string file) {
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData =  xmlLoader.load(file);
    for (auto [name, path]: xmlData.values) {
        if (!registerPath(name, path)) {
            LIA_error_f("Failed to register path {} for {}", path, name);
            return false;
        }
    }
    return true;
}

bool LIA::ObjectManager::registerTemplatePathsFromFile(std::string file) {
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData =  xmlLoader.load(file);
    for (auto [name, path]: xmlData.values) {
        if (!registerTemplatePath(name, path)) {
            LIA_error_f("Failed to register template path {} for {}", path, name);
            return false;
        }
    }
    return true;
}