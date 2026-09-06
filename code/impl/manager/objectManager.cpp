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
    LIA_trace_f("Created new object. Current size {}", _objects.size());
    return indx;
}

bool LIA::ObjectManager::remove(std::vector<std::string> &toRemove) {
    LIA_TRY
        for (auto name: toRemove) {
            int index = getByName(-1, name);
            _objects.erase(_objects.begin() + index);
            LIA_trace_f("Removed {} from index {}", name, index);
        }
        return true;
    LIA_CATCH_RETURN_FALSE
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
        std::string materialLib = xmlLoader.getString(xmlObjectData, "materialLib", "");

        objectId = create();
        Object* object = get(objectId);
        object->_name = oName;
        object->_materialLib = materialLib;
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

#include "manager/textureManager.hpp"
#include "manager/materialManager.hpp"
#include "Engine.hpp"
bool LIA::ObjectManager::load(ShaderManager* shaderManager) {
    LIA_TRY
        LIA_info("Loading objects");
        if (!_modelManager.load(shaderManager)) {
            LIA_error("Failed to load models");
            return false;
        }
        for (Object &object: _objects) {
            Model* model = _modelManager.get(object._modelInfo.id);
            ModelInfo* info = &object._modelInfo;

            if (model->size == 0) {
                LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
                return false;
            } else if (!model->isInGpu) {
                LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
                return false;
            } else {
                LIA_trace(std::vformat("Object {} loaded", std::make_format_args(object._name)));
            }
            if (object._materialLib != "" && object._materialLib.compare(model->materialLib) != 0) {
                if(!loadMaterials(object, model)) {
                    LIA_error_f("Failed to load all materials for {}", object._name);
                    return false;
                }
            }
        }
        _modelManager.logNameToModelMap();
        _modelManager.logModels();
        LIA_debug_f("Objects.size: {}, Models.size: {}", _objects.size(), _modelManager.size());
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::ObjectManager::loadMaterials(Object& object, Model* model) {
    LIA_trace_f("Loading material lib {} for object {}", object._materialLib, object._name);
    // Load materials
    LIA::MaterialManager &materialManager = LIA::Engine::getInstance().getMaterialManager();
    if (!materialManager.registerMaterialLib(object._materialLib, model->folder)) {
        LIA_fatal_f("Cannot register material lib {}", object._materialLib);
        return false;
    }
    if (!materialManager.loadLib(object._materialLib)) {
        LIA_fatal_f("Cannot load material lib {}", object._materialLib);
        return false;
    }
    // Load textures
    LIA::TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();
    for (std::string materialName: model->data.materialName) {
        LIA::Material &material = materialManager.getByLib(object._materialLib, materialName);
        LIA_TRY
            if (material.hasTexture) {
                if (!textureManager.registerTexture(material.texture, material.folder, material.texture, TextureType::RGBA)) {
                    LIA_error_f("Failed to register texture {} for material {}", material.texture, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasTexture ? material.texture : "empty");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            if (!material.hasTexture) {
                material.texture = "empty";
                material.hasTexture = true;
            }
        LIA_CATCH_RETURN_FALSE

        LIA_TRY
            if (material.hasBump) {
                if (!textureManager.registerTexture(material.bumpTexture, material.folder, material.bumpTexture, TextureType::ANY)) {
                    LIA_error_f("Failed to register bump texture {} for material {}", material.bumpTexture, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasBump ? material.bumpTexture : "emptyBump");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load bump texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            if (!material.hasBump) {
                material.bumpTexture = "emptyBump";
                material.hasBump = true;
            }
        LIA_CATCH_RETURN_FALSE
                
        LIA_TRY
            if (material.hasEm) {
                if (!textureManager.registerTexture(material.emTexture, material.folder, material.emTexture, TextureType::HDR)) {
                    LIA_error_f("Failed to register em texture {} for material {}", material.emTexture, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasEm ? material.emTexture : "emptyEm");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load em texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            if (!material.hasEm) {
                material.emTexture = "emptyEm";
                material.hasEm = true;
            }
        LIA_CATCH_RETURN_FALSE
    }
    return true;
}

void LIA::ObjectManager::pass(Scene *scene) {
    for (Object &object: _objects) {
        if (object._hide) {
            continue;
        }
        Model* model = _modelManager.get(object._modelInfo.id);
        if (model->isInGpu) {
            std::vector<int> offsets = model->data.offsets;
            for (int i = 0; i < offsets.size(); i++) {
                int oSize = model->size - offsets[i];
                if (i + 1 < offsets.size()) {
                    oSize = offsets[i + 1] - offsets[i];
                }
                scene->add(
                    object._name,
                    object._position, object._rotation, object._scale,
                    model->vao, model->shader,
                    model->hasIndices, oSize,
                    object._materialLib != "" ? object._materialLib : model->materialLib,
                    model->data.materialName.size() <= i ? "" : model->data.materialName[i],
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

void LIA::ObjectManager::setModel(Object* object, std::string modelName) {
    LIA_TRY
        LIA_debug(std::vformat("Setting model {} to object {}", std::make_format_args(modelName, object->_name)));
        Model* model = _modelManager.create(modelName);
        object->_modelInfo.name = modelName;
        object->_modelInfo.id = model->indx;
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
        LIA_error_f("Failed to get path for '{}'", id);
        return false;
    }
    return loadObject(path, name);
}

bool LIA::ObjectManager::loadObject(std::string path, std::string customName) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlObject;
        if (_cache.contains(path)) {
            xmlObject = _cache.at(path);
        } else {
            xmlObject = xmlLoader.load(path);
            _cache.emplace(path, xmlObject);
        }
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
        if (object->_modelInfo.id < 0) {
            LIA_fatal_f("Failed to get info for the model of object {}", name);
            return false;
        }
        LIA_trace("Setting object type");
        setType(object, type);
        LIA_trace("Setting posititon");
        Position position = xmlLoader.getPositionDefault0(xmlObject);
        setPosition(object, position);
        LIA_trace("Setting scale");
        Scale scale = xmlLoader.getScaleDefault1(xmlObject);
        setScale(object, scale);
        LIA_trace("Setting rotation");
        Rotation rotation = xmlLoader.getRotationDefault0(xmlObject);
        rotation = convertRotation(rotation);
        setRotation(object, rotation);
        
        LIA_trace("Getting physics node");
        if (xmlLoader.hasNode(xmlObject, "physics")) {
            XmlLoader::XmlNode physicsNode = xmlLoader.getNode(xmlObject, "physics");
            object->_collison._grounded = xmlLoader.getBoolean(physicsNode, "grounded", "true");
            object->_physics._mass = xmlLoader.getFloat(physicsNode, "mass", 0.0f);
        }
        else {
            object->_collison._grounded = false;
            object->_physics._mass = 1;
        }
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

    if (model->size == 0) {
        LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
        return false;
    } else if (!model->isInGpu) {
        LIA_error(std::vformat("Object {} failed to load model {}", std::make_format_args(object._name, model->path)));
        return false;
    } else {
        LIA_trace(std::vformat("Object {} loaded", std::make_format_args(object._name)));
    }
    if (object._materialLib != "" && object._materialLib.compare(model->materialLib) != 0) {
        if(!loadMaterials(object, model)) {
            LIA_error_f("Failed to load all materials for {}", object._name);
            return false;
        }
    }
    _modelManager.logNameToModelMap();
    _modelManager.logModels();
    LIA_debug_f("Objects.size: {}, Models.size: {}", _objects.size(), _modelManager.size());
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