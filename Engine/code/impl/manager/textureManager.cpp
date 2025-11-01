#include "manager/textureManager.hpp"

bool LIA::TextureManager::init() {
    if (!registerTexture("empty", "./data/graphics/emptyTextures/", "Shader_NoneWhite.png", TextureType::RGBA)) {
        LIA_error("Failed to register empty texture");
        return false;
    }
    if (!registerTexture("emptyBump", "./data/graphics/emptyTextures/", "Shader_NoneNormal.png", TextureType::ANY)) {
        LIA_error("Failed to register emptyBump texture");
        return false;
    }
    if (!registerTexture("emptyEm", "./data/graphics/emptyTextures/", "Shader_NoneBlack.png", TextureType::HDR)) {
        LIA_error("Failed to register emptyEm texture");
        return false;
    }
    return true;
}

bool LIA::TextureManager::load() {
    if (!load("empty")) {
        LIA_error("Failed to load empty texture");
        return false;
    }
    if (!load("emptyBump")) {
        LIA_error("Failed to load emptyBump texture");
        return false;
    }
    if (!load("emptyEm")) {
        LIA_error("Failed to load emptyEm texture");
        return false;
    }
    return true;
}

LIA::Texture& LIA::TextureManager::get(std::string name) {
    LIA_trace_f("retrieving texture '{}'", name);
    return _textures[name];
}

bool LIA::TextureManager::load(std::string name) {
    LIA_TRY
        Texture& texture = get(name);
        if (texture.isValid()) {
            LIA_trace_f("[FOUND_VALID] {} = {}", texture._name, texture._id);
            return true;
        }
        LIA_trace_f("Loading texture {}", texture._name);
        std::string path = std::vformat("{}{}", std::make_format_args(texture._folder, texture._path)); 
        if (texture._path.ends_with(std::string_view(".bmp"))) {
            LIA_debug_f("{} with path {} is bmp", texture._name, texture._path);
            if (!TextureLoader::loadBMP(texture._id, path)) {
                LIA_error_f("Failed to load bmp texture {}", texture._name);
                return false;
            }
        } else {
            LIA_debug_f("{} with path {} is not bmp", texture._name, texture._path);
            if (texture._type == TextureType::ANY) {
                if (!TextureLoader::loadTexture(texture._id, path)) {
                    LIA_error_f("Failed to load texture {}", texture._name);
                    return false;
                }
            } else if (texture._type == TextureType::HDR) {
                if (!TextureLoader::loadTextureHDR(texture._id, path)) {
                    LIA_error_f("Failed to load hdr texture {}", texture._name);
                    return false;
                }    
            } else if (texture._type == TextureType::RGBA) {
                if (!TextureLoader::loadTextureRGBA(texture._id, path)) {
                    LIA_error_f("Failed to load rgba texture {}", texture._name);
                    return false;
                }    
            } else {
                LIA_fatal_f("Unknown texture type for {}", texture._name);
                return false;
            }
        }
        LIA_trace_f("New texture {} loaded with id {}", texture._name, texture._id);
        return true;
    LIA_CATCH_RETURN_FALSE
}

bool LIA::TextureManager::registerTexture(std::string name, std::string folder, std::string path, TextureType type) {
    if (_textures.find(name) == _textures.end()) {
        Texture tmp;
        _textures.emplace(std::pair<std::string, Texture> (name, tmp));
        _textureNames.emplace_back(name);
    } else {
        LIA_warn_f("Texture '{}' is already registered", name);
        return true;
    }
    Texture& texture = get(name);
    texture._folder = folder;
    texture._name = name;
    texture._path = path;
    texture._type = type;
    LIA_trace_f("Registered texture '{}' with path '{}' in folder '{}'", name, path, folder);
    return true;
}