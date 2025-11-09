#ifndef LIA_MATERIAL_MANAGER
#define LIA_MATERIAL_MANAGER

#include <vector>
#include <map>
#include <string>

#include "loaders/materialLoader.hpp"
#include "data/Material.hpp"
#include "data/MaterialLib.hpp"

namespace LIA {
    class MaterialManager {
        private:
            std::map<std::string, Material> _materials;
            std::vector<std::string> _materialNames;
            
            std::map<std::string, MaterialLib> _materialLibs;
            std::vector<std::string> _materialLibNames;

            MaterialLoader _loader;
        public:
            bool init();
            bool load();
            
            Material& get(std::string);
            /***
             * get material from specific librabry
             * @param {string} lib name
             * @param {string} material name
             */
            Material& getByLib(std::string, std::string);
            std::vector<std::string>& getNames() { return _materialNames; }
            std::vector<std::string>& getLibNames() { return _materialLibNames; }

            /***
             * @param {string} lib name
             */
            bool loadLib(std::string);
            /***
             * @param {string} material name
             */
            bool load(std::string);
            /***
             *  @param {string} name
             *  @param {string} folder
             *  @param {string} pathToFile
            */
            bool registerMaterial(std::string, std::string, std::string);
            /***
             * @param {string} name
             * @param {string} folder
             */
            bool registerMaterialLib(std::string, std::string);
    };
}
#endif