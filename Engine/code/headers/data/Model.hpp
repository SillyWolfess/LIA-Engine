#ifndef LIA_MODEL
#define LIA_MODEL

#include "./../precompiled.hpp"
#include <string>
#include <vector>

#include "ModelData.hpp"
#include "data/Texture.hpp"

namespace LIA {
    struct Model {
        ModelData data;

        std::string folder;
        std::string path;
        std::string name;
        std::string shader;
        
        std::vector<Texture> _textures;
        std::vector<Texture> _bump;
        std::vector<Texture> _em;
        
        std::vector<unsigned int> _indices;

        GLuint vao;
        GLuint vbo;
        GLuint program;
        
        int size;
        bool hasIndices;
        bool isInGpu;
        bool infoLoaded;
        int indx;
        #if LIA_DEBUG_ON
        Model();
        Model(const Model&);
        ~Model();
        #endif
    };
}
#endif