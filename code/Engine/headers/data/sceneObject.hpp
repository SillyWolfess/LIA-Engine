#ifndef LIA_SCENE_OBJECT_HPP
#define LIA_SCENE_OBJECT_HPP
#include <string>

#include "./../precompiled.hpp"
#include "position.hpp"
#include "Color.hpp"
#include "data/Texture.hpp"
#include "data/Model.hpp"

#define LIA_SCENE_OBJECT_DEBUG 0
namespace LIA {
    struct SceneObject {
        std::string _identifier;
        Position _position;
        Rotation _rotation;
        Scale _scale;
        Color _color;
        std::string _shader;

        Texture _texture;
        Texture _bumpTexture;
        Texture _emTexture;

        gMaterial _material;

        GLuint _vao;
        int _size;
        int _offset;

        bool _hasTexture;
        bool _hasBumpTexture;
        bool _hasEmTexture;
        
        bool _hasMaterial;
        
        bool _useIndices;
        bool _passColor;

        bool _drawAsPoints = false;
        bool _wireMode = false;

        #if LIA_DEBUG_ON && LIA_SCENE_OBJECT_DEBUG
        SceneObject();
        SceneObject(const SceneObject&);
        ~SceneObject();
        #endif
    };
}
#endif