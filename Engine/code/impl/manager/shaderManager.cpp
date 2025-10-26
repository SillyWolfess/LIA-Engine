#include "manager/shaderManager.hpp"
#include "precompiled.hpp"

bool LIA::ShaderManager::validate(ShaderProgram& shader) {
    GLuint internalId = getProgramId(shader.getName());
    if (shader.getProgram() != internalId) {
        LIA_fatal(std::vformat("Program Ids do not match. Program name = {}. Program Id = {}. Stored Id = {}.", std::make_format_args(shader.getName(), shader.getProgram(), internalId)));
        return false;
    }
    return true;
}

bool LIA::ShaderManager::init() {

    XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load("./data/settings/shaders.xml");
    
    _shaders.reserve(xmlData.values.size());
    for (auto [name, path] : xmlData.values) {
        XmlLoader::XmlData xmlShader = xmlLoader.load(path);
        std::map<std::string, std::string> shaderData = xmlShader.values;
        _shaders.emplace_back(name);
        int index = _shaders.size() - 1;
        _shaderMap.emplace(std::pair<std::string, int>(name, index));
        ShaderProgram& shp = _shaders[index];
        std::string vertexPath = shaderData.at("vertex");
        if (vertexPath.compare("") != 0) {
            shp.add(LIA_SHADER::VERTEX, vertexPath);
        }
        std::string fragmentPath = shaderData.at("fragment");
        if (fragmentPath.compare("") != 0) {
            shp.add(LIA_SHADER::FRAGMENT, fragmentPath);
        }
        std::string geomPath = shaderData.at("geom");
        if (geomPath.compare("") != 0) {
            shp.add(LIA_SHADER::GEOMETRY, geomPath);
        }
    }
    LIA_trace("Validating shaders");
    for (ShaderProgram &shader : _shaders) {
        if (!shader.load()) {
            LIA_fatal(std::vformat("{} failed to load", std::make_format_args(shader.getName())));
            return false;
        }
        if (!validate(shader)) {
            LIA_fatal(std::vformat("{} validation failed", std::make_format_args(shader.getName())));
            return false;
        }
    }
    LIA_trace("Validation passed");
    return true;
}

GLuint LIA::ShaderManager::getProgramId(std::string shaderName) {
    std::map<std::string, int>::iterator it = _shaderMap.find(shaderName);
    if (it == _shaderMap.end()) {
        return 0;
    }
    return _shaders[it->second].getProgram();
}

LIA::ShaderProgram& LIA::ShaderManager::getShader(std::string shaderName) {
    std::map<std::string, int>::iterator it = _shaderMap.find(shaderName);
    if (it == _shaderMap.end()) {
        return _shaders[0];
    }
    return _shaders[it->second];
}

LIA::ShaderManager::~ShaderManager() {
    LIA_trace("Destroying Shader manager");
    /*
    for (ShaderProgram &shader : _shaders) {
        shader.destroy();
    }
    */
    _shaders.clear();
    _shaderMap.clear();
}