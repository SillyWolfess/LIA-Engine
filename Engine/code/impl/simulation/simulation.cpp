#include "precompiled.hpp"

#include "simulation/simulation.hpp"
#include "loaders/objLoader.hpp"
#include "datastructure/grid.hpp"
#include "Engine.hpp"

bool LIA::Simulation::init() {
    LIA_info("Init simulation");
    _objManager = &(LIA::Engine::getInstance().getObjectManager());
    _eventManager = &(LIA::Engine::getInstance().getEventManager());
    _state = LIA_SIM_STATE::EMPTY;
    _gameState = LIA_GAME_STATE::RUNNING;
    _playerId = -1;
    return true;
}

void LIA::Simulation::reset() {
    _objManager->reset();
    _playerId = -1;
    _state = LIA_SIM_STATE::EMPTY;
    _gameState = LIA_GAME_STATE::RUNNING;
}

bool LIA::Simulation::load(ShaderManager* shaderManager) {
    if (_state == LIA_SIM_STATE::ENDED) {
        reset();
    }
    if (_state != LIA_SIM_STATE::EMPTY) {
        LIA_error("Simulation is already running. Cannot load.");
        return false;
    }
    _state = LIA_SIM_STATE::LOADING;
    _objManager->print();
    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load("./data/simulation/scene.xml");
    for (auto [name, path]: xmlData.values) {
        XmlLoader::XmlNode xmlTypeNode = xmlLoader.getNode(xmlData, name);
        std::string type = xmlLoader.getString(xmlTypeNode, "type");
        if (type.compare("grid") == 0) {
            Grid grid;
            if (!grid.init(path)) {
                LIA_error("Failed to initialize grid");
                return false;
            }
            if (!grid.load(*_objManager)) {
                LIA_error("Failed to create grid");
                return false;
            }
        } else if (type.compare("object") == 0) {
            if (!loadObject(path)) {
                LIA_error(std::vformat("Failed to load object from path {}", std::make_format_args(path)));
                return false;
            }
        } else if (type.compare("light") == 0) {
            LIA_trace("Reading light data");
            if (!loadLight(path)) {
                LIA_error_f("Failed to laod light data from path {}", path);
                return false;
            }            
        }
    }
    if (!_objManager->load(shaderManager)) {
        LIA_error("Failed to load objects");
        return false;
    }
    _state = LIA_SIM_STATE::LOADED;
    _gameState = LIA_GAME_STATE::RUNNING;
    _playerId = _objManager->getPlayer(-1);
    return true;
}

bool LIA::Simulation::update(AppWindow* appWindow, float deltaTime) {
    if (_state != LIA_SIM_STATE::RUNNING) {
        return true;
    }
    LIA_trace("Tick event trigger start");
        LIA::TickEvent tickEvent("game");
        _eventManager->handleEvent(tickEvent);
    LIA_trace("Tick event trigger end");
    return true;
}

LIA::Simulation::~Simulation() {
    LIA_trace("Destroying simulation");
}

void LIA::Simulation::passObjects(Scene* scene) {
    if (_state != LIA_SIM_STATE::RUNNING) {
        return;
    }
    _objManager->pass(scene);
    
    for (int indx = 0; indx < _lights.size(); indx++) {
        Light& _light = _lights[indx];
        if (_lightIds[indx] == -1) {
            _lightIds[indx] = scene->addLightSource();
        }
        Light& light = scene->getLight(_lightIds[indx]);
        copy(light._position, _light._position);
        copy(light._direction, _light._direction);
        copyColor(light._color, _light._color);
        light._type = _light._type;
        light._linear = _light._linear;
        light._quadratic = _light._quadratic;
        light._cutoff = _light._cutoff;
        light._intensity = _light._intensity;
        Scale scale = emptyScale();
        setAll(scale, 1.0f);
        if (light._type == LightType::POINT || light._type == LightType::FLASHLIGHT) {
            scene->addCube(light._position, scale, light._color);
        }
    }
}

bool LIA::Simulation::loadObject(std::string path) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlObject = xmlLoader.load(path);
        LIA_trace("Reading object name");
        std::string name = xmlLoader.getValue(xmlObject, "object");
        LIA_trace("Reading object type");
        std::string type = xmlLoader.getString(xmlObject, "type");
        LIA_trace("Creating object");
        int indx = _objManager->create(name);
        if (indx == -1) {
            return false;
        }
        Object* object = _objManager->get(indx);
        Model* model = _objManager->getModelManager()->get(object->_modelInfo.id);
        if (!model->infoLoaded) {
            LIA_fatal_f("Failed to get info for the model of object {}", name);
            return false;
        }
        LIA_trace("Setting object type");
        _objManager->setType(object, type);
        LIA_trace("Setting posititon");
        Position position = xmlLoader.getPosition(xmlObject);
        _objManager->setPosition(object, position);
        LIA_trace("Setting scale");
        Scale scale = xmlLoader.getScale(xmlObject);
        _objManager->setScale(object, scale);
        LIA_trace("Setting rotation");
        Rotation rotation = xmlLoader.getRotation(xmlObject);
        rotation = convertRotation(rotation);
        _objManager->setRotation(object, rotation);
        
        LIA_trace("Getting physics node");
        XmlLoader::XmlNode physicsNode = xmlLoader.getNode(xmlObject, "physics");
        object->_collison._grounded = xmlLoader.getBoolean(physicsNode, "grounded", "true");
        object->_physics._mass = xmlLoader.getFloat(physicsNode, "mass", 0.0f);
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::Simulation::loadLight(std::string path) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlLight = xmlLoader.load(path);
        LIA_trace("Reading position");
        Position position = xmlLoader.getPosition(xmlLight);
        LIA_trace("Reading direction");
        Direction direction = xmlLoader.getDirection(xmlLight);
        LIA_trace("Reading color");
        Color color = xmlLoader.getColor(xmlLight, "color");
        LIA_trace("Reading type");
        std::string type = xmlLoader.getString(xmlLight, "type"); 
        Light& _light = _lights.emplace_back();
        _lightIds.push_back(-1);
        if (type.compare("directional") == 0) {
            _light._type = LightType::DIRECTIONAL;
        }
        else if (type.compare("point") == 0) {
            _light._type = LightType::POINT;
        }
        else if (type.compare("flashlight") == 0) {
            _light._type = LightType::FLASHLIGHT;
        }
        else {
            LIA_warn("Light type not recognized. Setting directional type");
            _light._type = LightType::DIRECTIONAL;
        }
        copy(_light._position, position);
        copy(_light._direction, direction);
        copyColor(_light._color, color);
        _light._linear = xmlLoader.getFloat(xmlLight, "linear", 0.0);
        _light._quadratic = xmlLoader.getFloat(xmlLight, "quadratic", 0.0);
        _light._cutoff = xmlLoader.getFloat(xmlLight, "cufoff", 0.0);
        _light._intensity = xmlLoader.getFloat(xmlLight, "intensity", 1.0);
        LIA_debug_f("Light color {:.2f} x {:.2f} x {:.2f}", _light._color.r, _light._color.g, _light._color.b);
    LIA_CATCH_RETURN_FALSE
    return true;
}