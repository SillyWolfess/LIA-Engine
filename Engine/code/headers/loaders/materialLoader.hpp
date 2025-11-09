#ifndef LIA_MATERIAL_LOADER
#define LIA_MATERIAL_LOADER
#include "data/Material.hpp"
#include "data/MaterialLib.hpp"

namespace LIA {
    class MaterialLoader {
        public:
            /***
             * @param {Material} material
             */
            static bool loadFromLib(Material&);
            static bool loadLib(MaterialLib&);
    };
}
#endif