#include "precompiled.hpp"
#include "graphics/scene.hpp"

#include "graphics/glwrapper.hpp"
#include "Engine.hpp"

bool LIA::Scene::init() {
    LIA_info("Scene init");
    background.r = 0;
    background.g = 0;
    background.b = 0;
    background.a = 1;
    _isWireMode = false;
    _depthTest = true;
    _layerId = 0;
    _layerMap.clear();
    _storage.reserve(10);
    return true;
}

bool LIA::Scene::initPrefab(ShaderManager& shaderManager) {
    if (!prefabSquare(shaderManager)) {
        LIA_fatal("Failed to make square prefab");
        return false;
    }
    if (!prefabCube(shaderManager)) {
        LIA_fatal("Failed to make cube prefab");
        return false;
    }
    if (!prefabSprite(shaderManager)) {
        LIA_fatal("Failed to make sprite prefab");
        return false;
    }
    return true;
}

bool LIA::Scene::sendLightSource(ShaderProgram& shader) {
    if (_lightSources.size() == 0) {
        return true;
    }
    int i = 0;
    for (Light& light : _lightSources) {
        i++;
        if (!shader.sendLight(light)) {
            LIA_error_f("Failed to send light {} to gpu", i);
            return false;
        }
    }
    return true;
}

bool LIA::Scene::prepareShader(ShaderProgram& shader, glm::mat4& VP) {
    if (!shader.bind()) {
        LIA_error("Failed to bind shader");
        return false;
    }
    if (!shader.loadUniforms()) {
        LIA_error("Failed to load uniforms");
        return false;
    }
    if (!shader.sendVP(glm::value_ptr(VP))) {
        LIA_error("Failed to send VP to shader");
        return false;
    }
	glEnableVertexAttribArray(0);
    return true;
}

bool LIA::Scene::draw(Camera &camera, ShaderManager* shaderManager, Font* font) {
    glm::mat4 Projection = camera.getProjection();
    glm::mat4 View = camera.getView();
    glm::mat4 VP = Projection * View;

    if (!_depthTest) {
        if (_enableLogs) {
            LIA_debug("Disabling depth test");
        }
        glDisable(GL_DEPTH_TEST);
    }
    for (auto &[layerId, layer] : _layerMap) {
        if (_enableLogs) {
            LIA_debug_f("Drawing layer {}", layerId);
        }
        for (auto &[shaderName, objects] : layer._shaderObjectMap) {
            ShaderProgram& sp = shaderManager->getShader(shaderName);
            if (!prepareShader(sp, VP)) {
                LIA_fatal_f("Failed to prepare shader '{}'", shaderName);         
                if (!_depthTest) {
                    glEnable(GL_DEPTH_TEST);
                }
                return false;
            }
            if (!sendLightSource(sp)) {
                LIA_fatal_f("Failed to send light to shader '{}'", shaderName);        
                if (!_depthTest) {
                    glEnable(GL_DEPTH_TEST);
                }
                return false;
            }
            for (SceneObject &object: objects) {
                if (!drawVAO(object, sp, VP)) {
                    LIA_fatal_f("Failed to draw {} with shader {}", object._identifier, shaderName);
                    if (!_depthTest) {
                        glEnable(GL_DEPTH_TEST);
                    }
                    return false;
                }
            }
        }
        if (_enableLogs) {
            LIA_debug("Object drawing done");
            LIA_debug("Getting font shader");
        }
        ShaderProgram& fontShader = shaderManager->getShader("font");
        if (!fontShader.bind()) {
            LIA_error("Failed to bind shader");
            if (!_depthTest) {
                glEnable(GL_DEPTH_TEST);
            }
            return false;
        }
        if (_enableLogs) {
            LIA_debug("Loading font uniforms");
        }
        if (!fontShader.loadUniforms()) {
            LIA_error("Failed to load uniforms");
            if (!_depthTest) {
                glEnable(GL_DEPTH_TEST);
            }
            return false;
        }
        if (_enableLogs) {
            LIA_debug("Preparing font to draw");
        }
        font->prepareForDraw();
        if (_enableLogs) {
            LIA_debug("Sending VP to font shader");
        }
        if (!fontShader.sendVP(glm::value_ptr(VP))) {
            LIA_error("Failed to send VP to shader");
            if (!_depthTest) {
                glEnable(GL_DEPTH_TEST);
            }
            return false;
        }
        
        if (_enableLogs) {
            LIA_debug("Drawing text data");
        }
        for (Data& data: layer._text) {
            font->drawText(VP, data.position, data.color, data.size, data.text);
        }
        if (_enableLogs) {
            LIA_debug_f("Layer {} done", layerId);
        }
    }
    if (!_depthTest) {
        if (_enableLogs) {
            LIA_debug("Enablig depth test");
        }
        glEnable(GL_DEPTH_TEST);
    }
    return true;
}

bool LIA::Scene::drawVAO(SceneObject& object, ShaderProgram& shader, glm::mat4& VP) {
    if (object._size < 1) {
        LIA_warn_f("Trying to draw empty object {}", object._identifier);
        return false;
    }
    glm::mat4 M = glm::mat4(1.0f);

    Position pos = object._position;
    M = glm::translate(M, glm::vec3(pos.x, pos.y, pos.z));
    // rotate
    Rotation rot = object._rotation;
    M = glm::rotate(M, rot.x, glm::vec3(1.0, 0.0, 0.0));
    M = glm::rotate(M, rot.y, glm::vec3(0.0, 1.0, 0.0));
    M = glm::rotate(M, rot.z, glm::vec3(0.0, 0.0, 1.0));
    // scale
    Scale scale = object._scale;
    M = glm::scale(M, glm::vec3(scale.x, scale.y, scale.z));

    glm::mat4 MVP = VP * M;

    // Send data to shader
    if (!shader.sendMVP(glm::value_ptr(MVP))) {
        LIA_error("Failed to send MVP to shader");
        return false;
    }
    if (!shader.sendM(glm::value_ptr(M))) {
        LIA_error("Failed to send M to shader");
        return false;
    }

    if (object._passColor) {
        if (!shader.sendColor(object._color)) {
            LIA_error("Failed to send color to shader");
            return false;
        }
    }

    if (object._hasMaterial) {
        MaterialManager &materialManager = LIA::Engine::getInstance().getMaterialManager();
        if (!useMaterial(materialManager, shader, object._materialLib, object._materialName)) {
            LIA_error_f("Failed to use material {} from {}", object._materialName, object._materialLib);
            return false;
        }
    } else if (object._hasTexture) {
        TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();
        if (!useTexture(textureManager, shader, object._texture)) {
            LIA_error_f("Failed to use texture {}", object._texture);
            return false;
        }
        shader.unbindTexture("textureBump", 1);
        shader.unbindTexture("textureEm", 2);
    }
    else {
        shader.unbindTexture("textureDiff", 0);
        shader.unbindTexture("textureBump", 1);
        shader.unbindTexture("textureEm", 2);
    }

    glBindVertexArray( object._vao );
    if (catchGlError()) {
        LIA_error("Failed to bind vao");
        glBindVertexArray(0);
        return false;
    }

    int offset = object._offset;
    int size = object._size;
    if (object._wireMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (object._useIndices) {
        glDrawElements(object._drawAsPoints ? GL_POINTS : GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
    } else {
        glDrawArrays(object._drawAsPoints ? GL_POINTS : GL_TRIANGLES, offset, size);
    }
    if (object._wireMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (catchGlError()) {
        LIA_error("Failed to draw vao");
        glBindVertexArray(0);
        return false;
    }
    return true;
}

#define SCENE_TRACE 0

bool LIA::Scene::useTexture(TextureManager& textureManager, ShaderProgram& shader, std::string textureName) {
    LIA_TRY
        Texture& texture = textureManager.get(textureName);
        #if SCENE_TRACE == 1
            LIA_trace_f("Using texture {} from texture name {}", texture._name, textureName);
        #endif
        if (!shader.bindTexture("textureDiff", 0, texture._id, texture._name)) {
            return false;
        }
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::Scene::useMaterial(MaterialManager &materialManager, ShaderProgram &shader, std::string materialLib, std::string materialName) {
    #if SCENE_TRACE == 1
        LIA_trace_f("Using material {} from {}", materialName, materialLib);
    #endif
    LIA_TRY
        Material& material = materialManager.getByLib(materialLib, materialName);
        TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();

        if (!shader.sendMaterial(material)) {
            LIA_error_f("Failed to send material {} to gpu", material.name);
            return false;
        }

        if (material.hasTexture) {
            Texture &texture = textureManager.get(material.texture);
            #if SCENE_TRACE == 1
                LIA_trace_f("Using texture {} from material identifier {}", texture._name, material.texture);
            #endif
            if (!shader.bindTexture("textureDiff", 0, texture._id, texture._name)) {
                return false;
            }
        } else {
            shader.unbindTexture("textureDiff", 0);
        }

        if (material.hasBump) {
            Texture &texture = textureManager.get(material.bumpTexture);
            #if SCENE_TRACE == 1
                LIA_trace_f("Using bump texture {} from material identifier {}", texture._name, material.bumpTexture);
            #endif
            if (!shader.bindTexture("textureBump", 1, texture._id, texture._name)) {
                return false;
            }
        } else {
            shader.unbindTexture("textureBump", 1);
        }

        if (material.hasEm) {
            Texture &texture = textureManager.get(material.emTexture);
            #if SCENE_TRACE == 1
                LIA_trace_f("Using em texture {} from material identifier {}", texture._name, material.emTexture);
            #endif
            if (!shader.bindTexture("textureEm", 2, texture._id, texture._name)) {
                return false;
            }
        } else {
            shader.unbindTexture("textureEm", 2);
        }
        return true;
    LIA_CATCH_RETURN_FALSE
}

LIA::Scene::~Scene() {
    LIA_DEBUG_DESTRUCTOR("Scene");
    LIA_trace("Scene destroyed");
    _prefabs.clear();
}

LIA::Scene::Scene() {
    LIA_DEBUG_CONSTRUCTOR("Scene");
}

void LIA::Scene::clear() {
//    _shaderObjectMap.clear();
    _layerMap.clear();
    _layerId = 0;
//    clearText();
}

LIA::SceneObject& LIA::Scene::addToShaderMap(std::string shader) {
    int layerId = _layerId;
    if (_layerMap.find(layerId) == _layerMap.end()) {
        Layer emptyLayer;
        _layerMap.emplace(std::pair<int, Layer>(layerId, emptyLayer));
    }
    Layer& layer = _layerMap[layerId];
    if (layer._shaderObjectMap.find(shader) == layer._shaderObjectMap.end()) {
        std::vector<SceneObject> emptyVec;
        layer._shaderObjectMap.emplace(std::pair<std::string, std::vector<SceneObject>>(shader, emptyVec));
    }
    std::vector<SceneObject> &objects = layer._shaderObjectMap[shader];
    return objects.emplace_back();
}

bool LIA::Scene::objectFromPrefab(SceneObject& object, std::string prefabName) {
    if (_prefabs.find(prefabName) == _prefabs.end()) {
        return false;
    }
    Prefab& square = _prefabs[prefabName];
    if (!square._loaded) {
        return false;
    }
    object._passColor = square._passColor;
    object._vao = square._vao;
    object._shader = square._shader;
    object._size = square._size;
    object._useIndices = square._useIndices;
    return true;
}

LIA::SceneObject* LIA::Scene::get(int layerId, int indx, std::string shader) {
    LIA_ASSERT_F(layerId > -1 && layerId < _layerMap.size(), "Layer id {} is out of bounds for shader {} with index {}", layerId, shader, indx);
    LIA_ASSERT_F(_layerMap[layerId]._shaderObjectMap.find(shader) != _shaderObjectMap.end(), "shader {} not found", shader);
    LIA_ASSERT_F(indx > -1 && indx < _layerMap[layerId]._shaderObjectMap[shader].size(), "Object out of bounds for shader {} and index {}", shader, indx);
    return &_layerMap[layerId]._shaderObjectMap[shader][indx];
}

bool LIA::Scene::add(Position position, Rotation rotation, Scale scale, GLuint vao, std::string shader, bool indices, int size) {
    SceneObject &object = addToShaderMap(shader);    

    object._position = position;
    object._rotation = rotation;
    object._scale = scale;

    object._passColor = false;
    object._hasMaterial = false;

    object._vao = vao;
    object._shader = shader;
    object._size = size;
    object._offset = 0;
    object._useIndices = indices;
    return true;
}

bool LIA::Scene::add(
    std::string identifier,
    Position position, Rotation rotation, Scale scale,
    GLuint vao, std::string shader,
    bool indices, int size,
    std::string materialLib,
    std::string materialName,
    int offset
)
{
    return add(identifier, position, rotation, scale, vao, shader, indices, size, materialLib, materialName, offset, false);
}

bool LIA::Scene::add(
    std::string identifier,
    Position position, Rotation rotation, Scale scale,
    GLuint vao, std::string shader,
    bool indices, int size,
    std::string materialLib,
    std::string materialName,
    int offset,
    bool asPoints
) {
    LIA_TRY
        if (!add(position, rotation, scale, vao, shader, indices, size)) {
            return false;
        }
        int indx = _layerMap[_layerId]._shaderObjectMap[shader].size() - 1;
        SceneObject& object = _layerMap[_layerId]._shaderObjectMap[shader][indx];
        object._identifier = identifier;
        object._offset = offset;
        object._drawAsPoints = asPoints;
        object._materialLib = materialLib;
        object._materialName = materialName;

        object._hasMaterial = object._materialName != "";
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::Scene::addSprite(std::string identifer, Position position, Rotation rotation, Scale scale, Color color, std::string texture) {
    Prefab& sprite = _prefabs["sprite"];
    if (!sprite._loaded) {
        return false;
    }
    //Lazy texture loading
    TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();
    if (!textureManager.has(texture)) {
        if (!textureManager.registerTexture(texture, "./data/graphics/textures/", texture, TextureType::ANY)) {
            LIA_error_f("Failed to lazy register texture {} for {}", texture, identifer);
            return false;
        }
        if (!textureManager.load(texture)) {
            LIA_fatal_f("Failed to lazy load texture {} for {}", texture, identifer);
            LIA::Engine::getInstance().fatal();
            return false;
        }
    }

    SceneObject& object = addToShaderMap(sprite._shader);

    object._identifier = identifer;
    object._position = position;
    object._rotation = rotation;
    object._scale = scale;
    object._color = color;

    object._hasTexture = true;
    object._texture = texture;
    object._passColor = sprite._passColor;
    object._vao = sprite._vao;
    object._shader = sprite._shader;
    object._size = sprite._size;
    object._useIndices = sprite._useIndices;
    return true;
}

bool LIA::Scene::addSquare(Position position, Rotation rotation, Scale scale, Color color) {
    return addSquare("[square]", position, rotation, scale, color);
}
bool LIA::Scene::addSquare(std::string identifer, Position position, Rotation rotation, Scale scale, Color color) {
    Prefab& square = _prefabs["square"];
    if (!square._loaded) {
        return false;
    }
    SceneObject& object = addToShaderMap(square._shader);

    object._identifier = identifer;
    object._position = position;
    object._rotation = rotation;
    object._scale = scale;
    object._color = color;

    object._passColor = square._passColor;
    object._vao = square._vao;
    object._shader = square._shader;
    object._size = square._size;
    object._useIndices = square._useIndices;
    return true;
}


bool LIA::Scene::addSquare(SceneObject& object, Position position, Rotation rotation, Scale scale, Color color) {
    if (!objectFromPrefab(object, "square")) {
        return false;
    }
    object._position = position;
    object._rotation = rotation;
    object._scale = scale;
    object._color = color;
    return true;
}

bool LIA::Scene::addCube(Position position, Scale scale, Color color) {
    Prefab& cube = _prefabs["cube"];
    if (!cube._loaded) {
        return false;
    }
    SceneObject &object = addToShaderMap(cube._shader);
    object._identifier = "[cube]";
    object._position = position;
    object._rotation = emptyRotation();
    object._scale = scale;
    object._color = color;

    object._passColor = cube._passColor;
    object._vao = cube._vao;
    object._shader = cube._shader;
    object._size = cube._size;
    object._useIndices = cube._useIndices;
    return true;
}

void LIA::Scene::clearText() {
    /*
	int size = _data.size();
	_data.clear();
	_data.reserve(size);
    */
}
void LIA::Scene::addText(Font* font, std::string text, Position position) {
	addText(font, text, position, font->getDefaultColor(), font->getDefaultSize());
}
void LIA::Scene::addText(Font* font, std::string text, Position position, Color color) {
	addText(font, text, position, color, font->getDefaultSize());
}
void LIA::Scene::addText(Font* font, std::string text, Position position, int size) {
	addText(font, text, position, font->getDefaultColor(), size);
}
void LIA::Scene::addText(Font* font, std::string text, Position position, Color color, int size) {
    int layerId = _layerId;
    if (_layerMap.find(layerId) == _layerMap.end()) {
        Layer emptyLayer;
        _layerMap.emplace(std::pair<int, Layer>(layerId, emptyLayer));
    }
    Layer& layer = _layerMap.at(layerId);
	Data& data = layer._text.emplace_back();
	data.text = text;
	data.position = position;
	data.color = color;
	data.size = size;
}

int LIA::Scene::addLightSource() {
    Light& light = _lightSources.emplace_back();
    return _lightSources.size() - 1;
}

LIA::SceneObject& LIA::Scene::getFromStorage(std::string name) {
    if (_storageMap.find(name) == _storageMap.end()) {
        _storage.emplace_back();
        _storageMap.emplace(std::pair<std::string, int>(name, _storage.size() - 1));
    }
    LIA_DEBUG_MAX_COUNT("Scene::_storage.maxSize", _storage.size());
    return _storage[_storageMap[name]];
}

#include "graphics/VertexBuffer.hpp"
#include "graphics/VertexArrayObject.hpp"
#include "data/Model.hpp"
#include "loaders/objLoader.hpp"
bool LIA::Scene::prefabSquare(ShaderManager& shaderManager) {
    LIA_TRY
        Prefab tmp;
        _prefabs.emplace(std::pair<std::string, Prefab>("square", tmp));
        Prefab& square = _prefabs["square"];
        square._loaded = false;
        square._passColor = true;
        square._shader = "gui";

        GLuint programId = shaderManager.getProgramId(square._shader);
        Model model;
        if (!makePrefab(programId, model, "./data/graphics/prefabs/square.obj")) {
            LIA_fatal("Failed to make square prefab");
            return false;
        }

        square._size = model.size;
        square._useIndices = model.hasIndices;
        square._vao = model.vao;
        square._loaded = true;
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::Scene::prefabSprite(ShaderManager& shaderManager) {
    LIA_TRY
        Prefab tmp;
        _prefabs.emplace(std::pair<std::string, Prefab>("sprite", tmp));
        Prefab& sprite = _prefabs["sprite"];
        sprite._loaded = false;
        sprite._passColor = true;
        sprite._shader = "sprite";

        GLuint programId = shaderManager.getProgramId(sprite._shader);
        Model model;
        if (!makePrefab(programId, model, "./data/graphics/prefabs/sprite.obj", true)) {
            LIA_fatal("Failed to make sprite prefab");
            return false;
        }

        sprite._size = model.size;
        sprite._useIndices = model.hasIndices;
        sprite._vao = model.vao;
        sprite._loaded = true;
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::Scene::prefabCube(ShaderManager& shaderManager) {
    LIA_TRY
        Prefab tmp;
        _prefabs.emplace(std::pair<std::string, Prefab>("cube", tmp));
        Prefab& cube = _prefabs["cube"];
        cube._loaded = false;
        cube._passColor = true;
        cube._shader = "gui";

        GLuint programId = shaderManager.getProgramId(cube._shader);
        Model model;
        if (!makePrefab(programId, model, "./data/graphics/prefabs/cube.obj")) {
            LIA_fatal("Failed to make cube prefab");
            return false;
        }

        cube._size = model.size;
        cube._useIndices = model.hasIndices;
        cube._vao = model.vao;
        cube._loaded = true;
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::Scene::makePrefab(GLuint programId, Model& model, const char* path, bool hasUvs) {
    ObjLoader::load(
        model.data,
        "./data/graphics/prefabs/",
        path
    );
    if (!model.data._complete) {
        LIA_fatal("Failed to load prefab");
        return false;
    }
    model.program = programId;
    model.hasIndices = false;
    model.size = model.data.vertices.size();

    glUseProgram(model.program);
    VertexArrayObject vertexArrayObject;
    if (!vertexArrayObject.generate()) {
        return false;
    }
    if (!vertexArrayObject.bind()) {
        vertexArrayObject.destroy();
        return false;
    }
    VertexBuffer& vertexBuffer = vertexArrayObject.getVertexBuffer();
    if (!vertexBuffer.generate()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.bind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.setData(model.data.vertices)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.enableAttributeArray(0)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.enableAttributePointer(0)) {
        vertexArrayObject.destroy();
        return false;
    }
    
    if (hasUvs) {
        if (model.data.uvs.size() != model.data.vertices.size()) {
            LIA_fatal_f("Model is malformed, vertices({}) != uvs({})", model.data.vertices.size(), model.data.uvs.size());
            vertexArrayObject.destroy();
            LIA::Engine::getInstance().fatal();
            return false;
        }
        VertexBuffer& uvBuffer = vertexArrayObject.getUvBuffer();
        if (!uvBuffer.generateAndBind()) {
            vertexArrayObject.destroy();
            return false;
        }
        if (!uvBuffer.setData(model.data.uvs)) {
            vertexArrayObject.destroy();
            return false;
        }
        if (!uvBuffer.enableAttributeArray(1)) {
            vertexArrayObject.destroy();
            return false;
        }
        if (!uvBuffer.enableAttributePointer(1, 2)) {
            vertexArrayObject.destroy();
            return false;
        }
    }
    
    model.vao = vertexArrayObject.getId();
    model.vbo = vertexBuffer.getId();
	model.data.vertices.clear();
	model.data.colours.clear();
    model.data.uvs.clear();
    LIA_debug(std::vformat("Vao {} and Vbo {} created with size {}", std::make_format_args(model.vao,  model.vbo, model.size)));
    return true;
}