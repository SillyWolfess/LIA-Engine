#ifndef LIA_MATERIAL_MANAGER
#define LIA_MATERIAL_MANAGER

#include <vector>
#include <map>
#include <string>

#include "loaders/materialLoader.hpp"
#include "data/Material.hpp"

namespace LIA {
    class MaterialManager {
        private:
            std::map<std::string, Material> _materials;
            std::vector<std::string> _materialNames;
            MaterialLoader _loader;
        public:
            bool init();
            bool load();
            Material& get(std::string);
            std::vector<std::string>& getNames() { return _materialNames; }
            bool load(std::string);
            /*
                @param {string} name
                @param {string} folder
                @param {string} pathToFile
            */
            bool registerMaterial(std::string, std::string, std::string);
    };
}
#endif