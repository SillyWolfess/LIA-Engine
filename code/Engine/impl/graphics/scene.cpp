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
//    _objects.reserve(10);
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
    return true;
}

bool LIA::Scene::sendLightSource(ShaderProgram& shader) {
    if (_lightSources.size() == 0) {
        return true;
    }
    for (Light& light : _lightSources) {
        if (!shader.sendLight(light)) {
            LIA_error_f("Failed to send light {} to gpu", 0);
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
}

bool LIA::Scene::draw(Camera &camera, ShaderManager* shaderManager) {
    glm::mat4 Projection = camera.getProjection();
    glm::mat4 View = camera.getView();
    glm::mat4 VP = Projection * View;

    for (auto &[shaderName, objects] : _shaderObjectMap) {
        ShaderProgram& sp = shaderManager->getShader(shaderName);
        if (!prepareShader(sp, VP)) {
            LIA_fatal_f("Failed to prepare shader '{}'", shaderName);
            return false;
        }
        if (!sendLightSource(sp)) {
            LIA_fatal_f("Failed to send light to shader '{}'", shaderName);
            return false;
        }
        for (SceneObject &object: objects) {
            if (!drawVAO(object, sp, VP)) {
                LIA_fatal_f("Failed to draw {} with shader {}", object._identifier, shaderName);
                return false;
            }
        }
    }
    /*
    for (SceneObject &object : _objects) {
        if (!drawObject(VP, object, shaderManager)) {
            LIA_fatal_f("Failed to draw {}", object._identifier);
            return false;
        }
    }
    */
    return true;
}
/*
bool LIA::Scene::drawObject(glm::mat4& VP, SceneObject& object, ShaderManager* shaderManager) {
    ShaderProgram& sp = shaderManager->getShader(object._shader);
    return drawVAO(object, sp, VP);
}
*/
/*
LIA::Scene::ShaderData LIA::Scene::setShaderProgram(SceneObject& object, ShaderManager* shaderManager) {
    ShaderData shaderData;
    shaderData.program = shaderManager->getProgramId(object._shader);
    return shaderData;
}
*/
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

    /*
    if (!shader.bind()) {
        return false;
    }
    // Get ids from shader
    if (!shader.loadUniforms()) {
        LIA_error("Failed to load uniforms");
        return false;
    }
    */
    // Send data to shader
    if (!shader.sendMVP(glm::value_ptr(MVP))) {
        LIA_error("Failed to send MVP to shader");
        return false;
    }
    if (!shader.sendM(glm::value_ptr(M))) {
        LIA_error("Failed to send M to shader");
        return false;
    }
    /*
    if (!shader.sendVP(glm::value_ptr(VP))) {
        LIA_error("Failed to send VP to shader");
        return false;
    }
    */
    if (object._passColor) {
        if (!shader.sendColor(object._color)) {
            LIA_error("Failed to send color to shader");
            return false;
        }
    }

    if (object._hasMaterial) {
        gMaterial& material = object._material;
        if (!shader.sendMaterial(material)) {
            LIA_error_f("Failed to send material {} to gpu", material.name);
            return false;

        }
    }
    if (object._hasTexture) {
        if (!shader.bindTexture("textureDiff", 0, object._texture._id, object._texture._name)) {
            return false;
        }
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureDiff"), 0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, object._texture._id);
         if (catchGlError()) {
            LIA_error_f("Failed to bind texture {}", object._texture._name);
            return false;
        }
        */
    } else {
        shader.unbindTexture("textureDiff", 0);
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureDiff"), 0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        */
    }

    if (object._hasBumpTexture) {
        if (!shader.bindTexture("textureBump", 1, object._bumpTexture._id, object._bumpTexture._name)) {
            return false;
        }
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureBump"), 1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, object._bumpTexture._id);
        if (catchGlError()) {
            LIA_error_f("Failed to bind texture {}", object._bumpTexture._name);
            return false;
        }
        */
    } else {
        shader.unbindTexture("textureBump", 1);
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureBump"), 1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, 0);
        */
    }

    if (object._hasEmTexture) {
        if (!shader.bindTexture("textureEm", 2, object._emTexture._id, object._emTexture._name)) {
            return false;
        }
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureEm"), 2);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, object._emTexture._id);
        if (catchGlError()) {
            LIA_error_f("Failed to bind texture {}", object._emTexture._name);
            return false;
        }
        */
    } else {
        shader.unbindTexture("textureEm", 2);
        /*
        glUniform1i(glGetUniformLocation(shader.getProgram(), "textureEm"), 2);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, 0);
        */
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
    /* else if (!_isWireMode && object._wireMode) {
        _isWireMode = true;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    */
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

LIA::Scene::~Scene() {
    LIA_DEBUG_DESTRUCTOR("Scene");
    LIA_trace("Scene destroyed");
    _prefabs.clear();
}

LIA::Scene::Scene() {
    LIA_DEBUG_CONSTRUCTOR("Scene");
}

void LIA::Scene::clear() {
    /*
    int size = _objects.size();
    LIA_DEBUG_MAX_COUNT("Scene::_objects.maxSize", size);
    _objects.clear();
    _objects.reserve(size > 0 ? size : 10);
    */
    _shaderObjectMap.clear();
}
/*
bool LIA::Scene::add(SceneObject& object) {
//    _objects.emplace_back(object);
    addToShaderMap(object);
    return true;
}
*/
LIA::SceneObject& LIA::Scene::addToShaderMap(std::string shader) {
    if (_shaderObjectMap.find(shader) == _shaderObjectMap.end()) {
        std::vector<SceneObject> emptyVec;
        _shaderObjectMap.emplace(std::pair<std::string, std::vector<SceneObject>>(shader, emptyVec));
    }
    std::vector<SceneObject> &objects = _shaderObjectMap[shader];
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

LIA::SceneObject* LIA::Scene::get(int indx, std::string shader) {
    LIA_ASSERT(_shaderObjectMap.find(shader) != _shaderObjectMap.end(), "shader not found");
    LIA_ASSERT(indx > -1 && indx < _shaderObjectMap[shader].size(), "Object out of bounds");
    return &_shaderObjectMap[shader][indx];
}

bool LIA::Scene::add(Position position, Rotation rotation, Scale scale, GLuint vao, std::string shader, bool indices, int size) {
//    SceneObject &object = _objects.emplace_back();
    SceneObject &object = addToShaderMap(shader);    

    object._position = position;
    object._rotation = rotation;
    object._scale = scale;

    object._passColor = false;
    object._hasMaterial = false;
    
    object._hasTexture = false;
    object._hasBumpTexture = false;
    object._hasEmTexture = false;

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
    std::vector<LIA::gMaterial>& materials, std::vector<int>& mIds,
    std::vector<Texture>& textures, std::vector<Texture>& bump, std::vector<Texture>& em,
    int offset
)
{
    return add(identifier, position, rotation, scale, vao, shader, indices, size, materials, mIds, textures, bump, em, offset, false);
}

bool LIA::Scene::add(
    std::string identifier,
    Position position, Rotation rotation, Scale scale,
    GLuint vao, std::string shader,
    bool indices, int size,
    std::vector<LIA::gMaterial>& materials, std::vector<int>& mIds,
    std::vector<Texture>& textures, std::vector<Texture>& bump, std::vector<Texture>& em,
    int offset, bool asPoints
) {
    if (!add(position, rotation, scale, vao, shader, indices, size)) {
        return false;
    }
//    int indx = _objects.size() - 1;
//    SceneObject& object = _objects[indx];
    int indx = _shaderObjectMap[shader].size() - 1;
    SceneObject& object = _shaderObjectMap[shader][indx];
    object._identifier = identifier;
    object._offset = offset;
    object._drawAsPoints = asPoints;

    TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();
    if (materials.size() > 0 && mIds.size() > offset) {
        int mId = mIds[offset];
        if (materials.size() <= mId) {
            return true;
        }
        object._material = materials[mId];
        object._hasMaterial = true;
        int tIndex = object._material.textureIndex;
        if (tIndex < 0) {
            return true;
        }
        if (textures.size() > tIndex /* && textures[tIndex].isValid()*/) {
            Texture& mTexture = textureManager.get(textures[tIndex]._name);
            if (mTexture.isValid()) {
                object._texture = mTexture;
                object._hasTexture = true;    
            }
            /*
            object._texture = textures[tIndex];
            object._hasTexture = true;
            */
        }
        if (bump.size() > tIndex/* && bump[tIndex].isValid()*/) {
            Texture& mTexture = textureManager.get(bump[tIndex]._name);
            if (mTexture.isValid()) {
                object._bumpTexture = mTexture;
                object._hasBumpTexture = true;    
            }
            /*
            object._bumpTexture = bump[tIndex];
            object._hasBumpTexture = true;
            */
        }
        if (em.size() > tIndex/* && em[tIndex].isValid()*/) {
            Texture& mTexture = textureManager.get(em[tIndex]._name);
            if (mTexture.isValid()) {
                object._emTexture = mTexture;
                object._hasEmTexture = true;    
            }
            /*
            object._emTexture = em[tIndex];
            object._hasEmTexture = true;
            */
        }
    }
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
//    SceneObject &object = _objects.emplace_back();
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
//    SceneObject &object = _objects.emplace_back();
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
        if (!makePrefab(programId, model, "./data/default/prefabs/square.obj")) {
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
        if (!makePrefab(programId, model, "./data/default/prefabs/cube.obj")) {
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

bool LIA::Scene::makePrefab(GLuint programId, Model& model, const char* path) {
    ObjLoader::load(
        model.data,
        "./data/default/prefabs/",
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
    
    model.vao = vertexArrayObject.getId();
    model.vbo = vertexBuffer.getId();
	model.data.vertices.clear();
	model.data.colours.clear();
    LIA_debug(std::vformat("Vao {} and Vbo {} created with size {}", std::make_format_args(model.vao,  model.vbo, model.size)));
    return true;
}