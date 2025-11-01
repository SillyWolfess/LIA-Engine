#include "manager/modelManager.hpp"

#include "Engine.hpp"
#include "logs.hpp"
#include "loaders/objLoader.hpp"
#include "loaders/textureLoader.hpp"

#include <GL/glew.h>
#include <gl/gl.h>
#include <glm/glm.hpp>

bool LIA::ModelManager::catchGlError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        char error[500];
        sprintf(error,"GL ERROR: code %d", err);
        LIA_error(error);
        return true;
    }
    while(glGetError() != GL_NO_ERROR){ };
    return false;
}

void LIA::ModelManager::logNameToModelMap() {
    for (std::map<std::string, int>::iterator it = _nameToModel.begin(); it != _nameToModel.end(); it++) {
        LIA_trace(std::vformat("model[{}] = '{}'", std::make_format_args(it->second, it->first)));
    }
}

void LIA::ModelManager::logModels() {
    for (Model& model: _models) {
        LIA_trace(std::vformat("model[{}].path = '{}'", std::make_format_args(model.indx, model.path)));
    }
}

LIA::Model* LIA::ModelManager::create(std::string modelName) {
    int indx = find(modelName);
    if (indx != -1) {
        return get(indx);
    }
    Model* model = create();
    model->name = modelName;
    model->infoLoaded = false;
    _nameToModel.emplace(std::pair<std::string, int>(modelName, model->indx));
    LIA_TRY
        std::string modelPathDef = getPath(modelName);
        XmlLoader xmlLoader;
        if (modelPathDef.compare("") == 0) {
            // Get data from xml
            XmlLoader::XmlData xmlData = xmlLoader.load("./data/models/list.xml");
            modelPathDef = xmlData.values.at(modelName);
        }
        XmlLoader::XmlData xmlModelData = xmlLoader.load(modelPathDef);
        model->folder =  xmlLoader.getString(xmlModelData, "folder");
        model->path = xmlLoader.getString(xmlModelData, "path");
        model->shader = xmlLoader.getString(xmlModelData, "shader");
        model->infoLoaded = true;
    LIA_CATCH_EMPTY
    return model;
}

LIA::Model* LIA::ModelManager::create() {
    LIA_debug("Creating model");
    //Model model;
    _models.emplace_back();
    Model* result = &_models[_models.size() - 1];
    result->indx = _models.size() - 1;
    result->isInGpu = false;
    result->infoLoaded = false;
    LIA_trace(std::vformat("Created new model at index {}", std::make_format_args(result->indx)));
    return result;
}

int LIA::ModelManager::find(std::string modelName) {
    std::map<std::string, int>::iterator indx = _nameToModel.find(modelName);
    if (indx == _nameToModel.end()) {
        return -1;
    }
    return indx->second;
}

LIA::Model* LIA::ModelManager::get(int indx) {
    return &_models.at(indx < 0 ? 0 : indx);
}
/*
LIA::Model* LIA::ModelManager::getByPath(std::string path) {
    return get(find(path));
}
*/
LIA::Model* LIA::ModelManager::getByName(std::string modelName) {
    return get(find(modelName));
}

bool LIA::ModelManager::load(ShaderManager* shaderManager) {
    LIA_info("Loading models");
    for (Model &model : _models) {
        if (!loadModel(model.indx)) {
            return false;
        }
    }
    LIA_debug(std::vformat("Loaded {} models", std::make_format_args(_models.size())));
    return true;
}

bool LIA::ModelManager::loadModel(int id) {
    LIA_TRY
        Model& model = _models[id];
        if (model.data._complete && model.isInGpu) {
            LIA_trace_f("Model '{}' was already loaded", model.name);
            return true;
        }
        int programId = Engine::getInstance().getShaderManager().getProgramId(model.shader);// shaderManager->getProgramId(model.shader);
        if (!load(model, programId)) {
            LIA_error(std::vformat("Failed to load model {}", std::make_format_args(model.path)));
            return false;
        } else if (model.size == 0) {
            LIA_error(std::vformat("Model {} does not have vertices", std::make_format_args(model.path)));
            return false;
        } else if (!model.data._complete) {
            LIA_error(std::vformat("Model {} not loaded", std::make_format_args(model.path)));
            return false;
        } else if (!model.isInGpu) {
            LIA_error(std::vformat("Model {} was not sent to gpu", std::make_format_args(model.path)));
            return false;
        } else {
            LIA_trace(std::vformat("Model {} loaded", std::make_format_args(model.path)));
            return true;
        }
    LIA_CATCH_RETURN_FALSE
}

int LIA::ModelManager::size() {
    return _models.size();
}

#include "graphics/VertexBuffer.hpp"
#include "graphics/VertexArrayObject.hpp"
#include "manager/textureManager.hpp"
bool LIA::ModelManager::load(Model& model, int programId) {
    model.isInGpu = false;
    ObjLoader::load(
        model.data,
        model.folder,
        model.path
	);
    if (!model.data._complete) {
        LIA_fatal("Failed to load model. Model is not completly loaded");
        return false;
    }
    model.program = programId;
    model.hasIndices = false;
    model.size = model.data.vertices.size();
    
    ModelData& mData = model.data;
    computeTangentBasis(
        // inputs
        mData.vertices,
        mData.uvs,
        // outputs
        mData.tangents,
        mData.bitangents
    );

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
    if (!vertexBuffer.generateAndBind()) {
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
    
    VertexBuffer& colorBuffer = vertexArrayObject.getColorBuffer();
    if (!colorBuffer.generateAndBind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!colorBuffer.setData(model.data.colours)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!colorBuffer.enableAttributeArray(1)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!colorBuffer.enableAttributePointer(1)) {
        vertexArrayObject.destroy();
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
    if (!uvBuffer.enableAttributeArray(2)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!uvBuffer.enableAttributePointer(2, 2)) {
        vertexArrayObject.destroy();
        return false;
    }

    VertexBuffer& tBuffer = vertexArrayObject.getTBuffer();
    if (!tBuffer.generateAndBind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!tBuffer.setData(model.data.tangents)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!tBuffer.setupArrayAndPointer(3, 3)) {
        vertexArrayObject.destroy();
        return false;
    }

    VertexBuffer& btBuffer = vertexArrayObject.getBBuffer();
    if (!btBuffer.generateAndBind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!btBuffer.setData(model.data.bitangents)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!btBuffer.setupArrayAndPointer(4, 3)) {
        vertexArrayObject.destroy();
        return false;
    }

    VertexBuffer& normalBuffer = vertexArrayObject.getNormalBuffer();
    if (!normalBuffer.generateAndBind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!normalBuffer.setData(model.data.normals)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!normalBuffer.setupArrayAndPointer(5, 3)) {
        vertexArrayObject.destroy();
        return false;
    }

    model.vao = vertexArrayObject.getId();
    model.vbo = vertexBuffer.getId();

	model.data.vertices.clear();
	model.data.colours.clear();
    model.data.uvs.clear();
    model.data.bitangents.clear();
    model.data.tangents.clear();
    model.data.normals.clear();

    // Load textures
    TextureManager& textureManager = LIA::Engine::getInstance().getTextureManager();
    for (gMaterial& material: model.data.materials) {
        LIA_TRY
            if (material.hasTexture) {
                if (!textureManager.registerTexture(material.texture, model.folder, material.texture, TextureType::RGBA)) {
                    LIA_error_f("Failed to register texture {} for material {}", material.texture, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasTexture ? material.texture : "empty");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            Texture& texture = model._textures.emplace_back();
            material.textureIndex = model._textures.size() - 1;
            texture._folder = mTexture._folder;
            texture._name = mTexture._name;
            texture._path = mTexture._path;
            texture._id = mTexture._id;
            /*
            if (material.hasTexture) {
                texture._folder = model.folder;
                texture._name = material.texture;
                texture._path = material.texture;
            } else {
                texture._folder = "./data/emptyTextures/";
                texture._name = "empty";
                texture._path = "Shader_NoneWhite.png";
            }
            std::string path = std::vformat("{}{}", std::make_format_args(texture._folder, texture._path));
            if (!TextureLoader::loadTextureRGBA(texture._id, path)) {
                LIA_error_f("Failed to laod texture {} for material {}", texture._name, material.name);
                return false;
            }
            */
        LIA_CATCH_RETURN_FALSE

        LIA_TRY
            if (material.hasBump) {
                if (!textureManager.registerTexture(material.texture_bump, model.folder, material.texture_bump, TextureType::ANY)) {
                    LIA_error_f("Failed to register bump texture {} for material {}", material.texture_bump, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasBump ? material.texture_bump : "emptyBump");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load bump texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            Texture& textureBump = model._bump.emplace_back();
            textureBump._folder = mTexture._folder;
            textureBump._name = mTexture._name;
            textureBump._path = mTexture._path;
            textureBump._id = mTexture._id;
            /*
            if (material.hasBump) {
                textureBump._folder = model.folder;
                textureBump._name = material.texture_bump;
                textureBump._path = material.texture_bump;
            } else {
                textureBump._folder = "./data/emptyTextures/";
                textureBump._name = "emptyBump";
                textureBump._path = "Shader_NoneNormal.png";
            }
            std::string pathBump = std::vformat("{}{}", std::make_format_args(textureBump._folder, textureBump._path));
            if (!TextureLoader::loadTexture(textureBump._id, pathBump)) {
                LIA_error_f("Failed to laod texture {} for material {}", textureBump._name, material.name);
                return false;
            }
            */
        LIA_CATCH_RETURN_FALSE
        
        LIA_TRY
            if (material.hasEm) {
                if (!textureManager.registerTexture(material.texture_em, model.folder, material.texture_em, TextureType::HDR)) {
                    LIA_error_f("Failed to register em texture {} for material {}", material.texture_em, material.name);
                    return false;
                }
            }
            Texture& mTexture = textureManager.get(material.hasEm ? material.texture_em : "emptyEm");
            if (!textureManager.load(mTexture._name)) {
                LIA_error_f("Failed to load em texture {} for material {}", mTexture._name, material.name);
                return false;
            }
            Texture& textureEm = model._em.emplace_back();
            textureEm._folder = mTexture._folder;
            textureEm._name = mTexture._name;
            textureEm._path = mTexture._path;
            textureEm._id = mTexture._id;
            /*
            if (material.hasEm) {
                textureEm._folder = model.folder;
                textureEm._name = material.texture_em;
                textureEm._path = material.texture_em;
            } else {
                textureEm._folder = "./data/emptyTextures/";
                textureEm._name = "emptyEm";
                textureEm._path = "Shader_NoneBlack.png";
            }
            
            std::string pathEm = std::vformat("{}{}", std::make_format_args(textureEm._folder, textureEm._path));
            if (!TextureLoader::loadTextureHDR(textureEm._id, pathEm)) {
                LIA_error_f("Failed to laod texture {} for material {}", textureEm._name, material.name);
                return false;
            }
            */
        LIA_CATCH_RETURN_FALSE
    }

    model.isInGpu = true;
    LIA_debug(std::vformat("Vao {} and Vbo {} created with size {}", std::make_format_args(model.vao,  model.vbo, model.size)));
    return true;
}

/*************************************/
/*************************************/
/*****************MATH****************/
/*************************************/
/*************************************/
void LIA::ModelManager::computeTangentBasis(
    // inputs
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    // outputs
    std::vector<glm::vec3>& tangents,
    std::vector<glm::vec3>& bitangents
) {
    if (uvs.size() < vertices.size() || vertices.size() == 0) {
        return;
    }
    for (int i = 0; i < vertices.size(); i += 3){
        // Shortcuts for vertices
        glm::vec3& v0 = vertices[i + 0];
        glm::vec3& v1 = vertices[i + 1];
        glm::vec3& v2 = vertices[i + 2];
    
        // Shortcuts for UVs
        glm::vec2& uv0 = uvs[i + 0];
        glm::vec2& uv1 = uvs[i + 1];
        glm::vec2& uv2 = uvs[i + 2];
    
        // Edges of the triangle : position delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;
    
        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x) * r;
            
        // Set the same tangent for all three vertices of the triangle.
        // They will be merged later, in vboindexer.cpp
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        // Same thing for bitangents
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }  
}; 

bool LIA::ModelManager::registerPathsFromFile(std::string file) {
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

bool LIA::ModelManager::registerPath(std::string name, std::string path) {
    LIA_trace_f("Adding path '{}' for '{}'", path, name);
    _paths.emplace(std::pair<std::string, std::string>(name, path));
    return true;
}

std::string LIA::ModelManager::getPath(std::string name) {
    LIA_TRY
        return _paths[name];
    LIA_CATCH(return "";)
}
