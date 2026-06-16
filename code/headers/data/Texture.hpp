#ifndef LIA_TEXTURE_HPP
#define LIA_TEXTURE_HPP

#include "precompiled.hpp"
namespace LIA {
     enum TextureType {ANY, HDR, RGBA};
    struct Texture {
        GLuint _id = 0;
        std::string _name;
        std::string _path;
        std::string _folder;
        TextureType _type;
        bool isValid() { return _id != 0; }
    }; 
}
#endif