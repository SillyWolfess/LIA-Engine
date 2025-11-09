#ifndef LIA_MATERIAL_LIB
#define LIA_MATERIAL_LIB
#include "data/Material.hpp"

#include <string>
#include <map>

namespace LIA {
    struct MaterialLib {
        std::string _name;
        std::string _folder;
        std::map<std::string, Material> _materials;
        bool _loaded;
    };
}
#endif