#include "graphics/shaderProgram.hpp"
#include "loaders/shaderLoader.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "graphics/glwrapper.hpp"

LIA::ShaderProgram::ShaderProgram() {
    LIA_DEBUG_CONSTRUCTOR("ShaderProgram");
    LIA_trace("ShaderProgram defualt constructor called");
}

LIA::ShaderProgram::ShaderProgram(std::string name) {
    LIA_DEBUG_CONSTRUCTOR("ShaderProgram");
    LIA_trace("ShaderProgram constructor called");
    _name = name;
}
LIA::ShaderProgram::ShaderProgram(const ShaderProgram& shaderProgram) {
    LIA_DEBUG_CONSTRUCTOR_COPY("ShaderProgram");
    LIA_trace("ShaderProgram copy constructor called");
    _program = shaderProgram._program;
    _loaded = shaderProgram._loaded;
    _name = shaderProgram._name;
    for (auto [shader_type, path] : shaderProgram._shaderPaths) {
        _shaderPaths.emplace(std::pair<LIA_SHADER, std::string>(shader_type, path));
    }
}
void LIA::ShaderProgram::add(LIA_SHADER type, std::string path) {
    _shaderPaths.emplace(std::pair<LIA_SHADER, std::string>(type, path));
}

LIA::ShaderProgram::sShaderReturn LIA::ShaderProgram::createShader(std::string path, GLenum type) {
    return createShader(path.c_str(), type);
}

std::string LIA::ShaderProgram::getShaderPath(LIA_SHADER type) {
    std::map<LIA_SHADER, std::string>::iterator it = _shaderPaths.find(type);
    if (it == _shaderPaths.end()) {
        return std::string("");
    }
    return it->second;
}

LIA::ShaderProgram::sShaderReturn LIA::ShaderProgram::createShader(const char* path, GLenum type) {
    LIA::ShaderProgram::sShaderReturn result;
    result.success = false;
    LIA_trace("Create glShader");
    GLuint shaderId = glCreateShader(type);

    if (catchGlError()) {
        LIA_error("Failed to create shader");
        return result;
    }

    LIA_trace(std::vformat("Reading code {}", std::make_format_args(path)));
    std::string code;
    std::ifstream codeStream(path, std::ios::in);
    if (codeStream.is_open()) {
        std::string line = "";
        while (std::getline(codeStream, line)) {
            code += "\n" + line;
        }
        codeStream.close();
    } else {
        LIA_error(std::vformat("Failed to read the file {}", std::make_format_args(path)));
        glDeleteShader(shaderId);
        return result;
    }

    LIA_trace("Setting shader source");
    char const* sourcePointer = code.c_str();
    glShaderSource(shaderId, 1, &sourcePointer , NULL);
    LIA_trace("Compiling shader");
    glCompileShader(shaderId);
    
    
    LIA_trace("Checking shader");
    GLint Result  = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(Result == GL_FALSE)
    {
        std::vector<char> errorMessage(InfoLogLength);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &errorMessage[0]);
        LIA_error(std::vformat("{}", std::make_format_args(&errorMessage[0])));
        glDeleteShader(shaderId);
        return result;
    }

    if (catchGlError()) {
        LIA_error("Failed to compile shader");
        return result;
    }

    LIA_debug(std::vformat("Shader {} loaded with id {}", std::make_format_args(path, shaderId)));
    result.id = shaderId;
    result.success = true;
    return result;
}

bool LIA::ShaderProgram::buildProgram() {
    LIA_trace("Building program start");
    _program = 0;
    _loaded = false;
    
    LIA_trace("Getting vertex path");
    std::string vertexPath = getShaderPath(LIA_SHADER::VERTEX);
    bool hasVertex = vertexPath.compare("") != 0;

    LIA_trace("Getting fragment path");
    std::string fragmentPath = getShaderPath(LIA_SHADER::FRAGMENT);
    bool hasFragment = fragmentPath.compare("") != 0;

    LIA_trace("Getting geom path");
    std::string geomPath = getShaderPath(LIA_SHADER::GEOMETRY);
    bool hasGeom = geomPath.compare("") != 0;

    GLuint VertexShaderID = 0;
    GLuint FragmentShaderID = 0;
    GLuint GeomShaderID = 0;

    LIA_trace("Creating shaders");
    if (hasVertex) {
        LIA_info(std::vformat("Creating vertex shader {} for {}", std::make_format_args(vertexPath, _name)));
        sShaderReturn sReturn = createShader(vertexPath, GL_VERTEX_SHADER);
        if (!sReturn.success) {
            LIA_error(std::vformat("Failed to create vertex shader {} for program {}", std::make_format_args(vertexPath, _name)));
            return false;
        }
        VertexShaderID = sReturn.id;
        LIA_info(std::vformat("Loaded vertex shader {} with id {} for program {}", std::make_format_args(vertexPath, VertexShaderID, _name)));
    }

    if (hasFragment) {
        LIA_info(std::vformat("Creating fragment shader {} for {}", std::make_format_args(fragmentPath, _name)));
        sShaderReturn sReturn = createShader(fragmentPath, GL_FRAGMENT_SHADER);
        if (!sReturn.success) {
            LIA_error(std::vformat("Failed to create fragment shader {} for program {}", std::make_format_args(fragmentPath, _name)));
            return false;
        }
        FragmentShaderID = sReturn.id;
        LIA_info(std::vformat("Loaded fragment shader {} with id {} for program {}", std::make_format_args(fragmentPath, FragmentShaderID, _name)));
    }

    if (hasGeom) {
        LIA_info(std::vformat("Creating geom shader {} for {}", std::make_format_args(geomPath, _name)));
        sShaderReturn sReturn = createShader(geomPath, GL_GEOMETRY_SHADER);
        if (!sReturn.success) {
            LIA_error(std::vformat("Failed to create geom shader {} for program {}", std::make_format_args(geomPath, _name)));
            return false;
        }
        GeomShaderID = sReturn.id;
        LIA_info(std::vformat("Loaded geometry shader {} with id {} for program {}", std::make_format_args(geomPath, GeomShaderID, _name)));
    }


    LIA_trace("Creating program");
    GLuint ProgramID = glCreateProgram();
    LIA_trace(std::vformat("Program created {}", std::make_format_args(ProgramID)));
    if (hasVertex) {
        glAttachShader(ProgramID, VertexShaderID);
    }
    if (hasFragment) {
        glAttachShader(ProgramID, FragmentShaderID);
    }
    if (hasGeom) {
        glAttachShader(ProgramID, GeomShaderID);
    }
    LIA_trace(std::vformat("Linking program {}", std::make_format_args(ProgramID)));
    glLinkProgram(ProgramID);
    
    // Check the program
    GLint Result  = GL_FALSE;
    int InfoLogLength;
    LIA_trace(std::vformat("Checking program {}", std::make_format_args(ProgramID)));
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(Result == GL_FALSE)
    {
        std::vector<char> ProgramErrorMessage(glm::max(InfoLogLength, int(1)) );
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        LIA_error(std::vformat("{}", std::make_format_args(&ProgramErrorMessage[0])));
    }
    
    if (hasVertex) {
        glDeleteShader(VertexShaderID);
    }
    if (hasFragment) {
        glDeleteShader(FragmentShaderID);
    }
    if (hasGeom) {
        glDeleteShader(GeomShaderID);
    }
    if(Result == GL_FALSE) {
        glDeleteProgram(ProgramID);
        LIA_error(std::vformat("Building program failed for {}", std::make_format_args(ProgramID)));
        return false;
    }
    
    _program = ProgramID;
    _loaded = true;
    LIA_trace(std::vformat("Building program finished with id {}", std::make_format_args(_program)));
    return true;
}

bool LIA::ShaderProgram::load() {
    LIA_trace(std::vformat("Loading shader program for {}", std::make_format_args(_name)));
    if (!buildProgram()) {
        LIA_fatal(std::vformat("Failed to load {}", std::make_format_args(_name)));
        return false;
    }
    LIA_info(std::vformat("Shader program {} created with id {}", std::make_format_args(_name, _program)));
    return true;
}
/*
void LIA::ShaderProgram::destroy() {
    if (!_loaded) {
        return;
    }
    LIA_trace(std::vformat("Destroying shader program {}", std::make_format_args(_program)));
    glDeleteProgram(_program);
}
*/
LIA::ShaderProgram::~ShaderProgram() {
    LIA_DEBUG_DESTRUCTOR("ShaderProgram");
    LIA_trace("ShaderProgram destructor called");
    if (!_loaded) {
        return;
    }
    LIA_trace(std::vformat("Destroying shader program {}", std::make_format_args(_program)));
    glDeleteProgram(_program);
}

bool LIA::ShaderProgram::bind() {
    glUseProgram(_program);
    if (catchGlError()) {
        LIA_error(std::vformat("There was gl error when trying to use program {}", std::make_format_args(_program)));
        return false;
    }
    return true;
}
bool LIA::ShaderProgram::sendToGPU(GLint id, int count, GLboolean transpose, const GLfloat *value) {
    glUniformMatrix4fv(id, count, transpose, value);
    if (catchGlError()) {
        LIA_error(std::vformat("Failed to send {} data to gpu", std::make_format_args(id)));
        return false;
    }
    return true;
}
bool LIA::ShaderProgram::sendToGPU(GLint id, float v1, float v2, float v3, float v4) {
    glUniform4f(id, v1, v2, v3, v4);
    if (catchGlError()) {
        LIA_error(std::vformat("Failed to send {} data to gpu", std::make_format_args(id)));
        return false;
    }
    return true;
}
bool LIA::ShaderProgram::getUniform(GLint &id, std::string name) {
    if (_uniforms.find(name) != _uniforms.end()) {
        id = _uniforms[name];
        return true;
    }
    const char* cName = name.c_str();
    id = glGetUniformLocation(_program, cName);
    if (catchGlError()) {
        LIA_error_f("Failed to get {} from shader", name);
        return false;
    }
    _uniforms[name] = id;
    return true;
}

void LIA::ShaderProgram::setVec3(GLuint shader_program, const char* name, float v1, float v2, float v3) {
    glm::vec3 v(v1, v2, v3);
    GLint id;
    getUniform(id, name);
    glUniform3fv(id, 1, &v[0]);
}

void LIA::ShaderProgram::setFloat(GLuint shader_program, const char* name, float v) {
    GLint id;
    getUniform(id, name);
    glUniform1f(id, v);
}


bool LIA::ShaderProgram::loadUniforms() {
    if (!getUniform(_mvpId, "MVP")) {
        return false;
    }
    if (!getUniform(_vpId, "VPID")) {
        return false;
    }
    if (!getUniform(_mId, "M")) {
        return false;
    }
    if (!getUniform(_colorId, "colour")) {
        return false;
    }
    return true;
}

bool LIA::ShaderProgram::sendColor(Color& color) {
    return sendToGPU(_colorId, color.r, color.g, color.b, color.a);
}

bool LIA::ShaderProgram::sendMatrix(GLint id, GLfloat *value) {
    return sendToGPU(id, 1, GL_FALSE, value);
}

bool LIA::ShaderProgram::sendM(GLfloat *value) {
    return sendMatrix(_mId, value);
}

bool LIA::ShaderProgram::sendMVP(GLfloat *value) {
    return sendMatrix(_mvpId, value);
}

bool LIA::ShaderProgram::sendVP(GLfloat *value) {
    return sendMatrix(_vpId, value);
}

bool LIA::ShaderProgram::sendMaterial(gMaterial& material) {
    setVec3(_program, "material.ambient", material.Ka[0], material.Ka[1], material.Ka[2]);
    setVec3(_program, "material.diffuse", material.Kd[0], material.Kd[1], material.Kd[2]);
    setVec3(_program, "material.specular", material.Ks[0], material.Ks[1], material.Ks[2]);
    setVec3(_program, "material.emissive", material.Ke[0], material.Ke[1], material.Ke[2]);
    setFloat(_program, "material.shininess", material.Ns);
    if (catchGlError()) {
        LIA_error("Failed to send material to gpu");
        return false;
    }
    return true;
}

bool LIA::ShaderProgram::sendLight(Light& light) {
    if (light._type == LightType::DIRECTIONAL) {
        setVec3(_program, "light.direction", light._direction.x, light._direction.y, light._direction.z);
        setVec3(_program, "light.color", light._color.r, light._color.g, light._color.b);
        setFloat(_program, "light.intensity", light._intensity);
    } else if (light._type == LightType::POINT) {
        setVec3(_program, "pointLight.position", light._position.x, light._position.y, light._position.z);
        setVec3(_program, "pointLight.color", light._color.r, light._color.g, light._color.b);
        setFloat(_program, "pointLight.linear", light._linear);
        setFloat(_program, "pointLight.quadratic", light._quadratic);
        setFloat(_program, "pointLight.intensity", light._intensity);
    } else if (light._type == LightType::FLASHLIGHT) {
        setVec3(_program, "flashLight.position", light._position.x, light._position.y, light._position.z);
        setVec3(_program, "flashLight.direction", light._direction.x, light._direction.y, light._direction.z);
        setVec3(_program, "flashLight.color", light._color.r, light._color.g, light._color.b);
        setFloat(_program, "flashLight.cutoff", glm::cos(glm::radians(light._cutoff)));
        setFloat(_program, "flashLight.linear", light._linear);
        setFloat(_program, "flashLight.quadratic", light._quadratic);
        setFloat(_program, "flashLight.intensity", light._intensity);
    }
    if (catchGlError()) {
       LIA_error("Failed to send light to gpu");
       return false;
    }
    return true;
}

bool LIA::ShaderProgram::bindTexture(std::string uniformName, int uniformId, int textureId, std::string textureName) {
    GLint id;
    if (!getUniform(id, uniformName)) {
        LIA_error_f("Failed to get {} for {}", uniformName, textureName);
        return false;
    }
    glUniform1i(id, uniformId);
    glActiveTexture(GL_TEXTURE0 + uniformId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    if (catchGlError()) {
        LIA_error_f("Failed to bind {} at {} for texture {}", uniformName, uniformId, textureName);
       return false;
    }
    return true;
}

bool LIA::ShaderProgram::unbindTexture(std::string uniformName, int uniformId) {
    GLint id;
    if (!getUniform(id, uniformName)) {
        LIA_error_f("Failed to get {} for {}", uniformName);
        return false;
    }
    glUniform1i(id, uniformId);
    glActiveTexture(GL_TEXTURE0 + uniformId);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (catchGlError()) {
        LIA_error_f("Failed to unbind {} at {}", uniformName, uniformId);
       return false;
    }
    return true;
}