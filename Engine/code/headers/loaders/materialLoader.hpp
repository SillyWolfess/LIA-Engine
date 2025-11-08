#ifndef LIA_MATERIAL_LOADER
#define LIA_MATERIAL_LOADER
#include "data/Material.hpp"

namespace LIA {
    class MaterialLoader {
        public:
            /***
             * @param {Material} material
             */
            static bool load(Material&);
    };
}
#endif