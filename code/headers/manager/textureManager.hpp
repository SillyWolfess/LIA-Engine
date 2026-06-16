#ifndef LIA_TEXTURE_MANAGER
#define LIA_TEXTURE_MANAGER

#include <vector>
#include <map>
#include <string>

#include "data/Texture.hpp"
#include "loaders/textureLoader.hpp"

namespace LIA {
    class TextureManager {
        private:
            std::map<std::string, Texture> _textures;
            std::vector<std::string> _textureNames;
            TextureLoader _loader;
        public:
            bool init();
            bool load();
            Texture& get(std::string);
            std::vector<std::string>& getNames() { return _textureNames; }
            bool load(std::string);
            /*
                @param {string} name
                @param {string} folder
                @param {string} pathToFile
                @param {TextureType} type
            */
            bool registerTexture(std::string, std::string, std::string, TextureType);
    };
}
#endif