#ifndef LIA_OBJ_LOADER
#define LIA_OBJ_LOADER

#include "./../data/ModelData.hpp"
#include <string>

#define MIN_VALUE -100000
#define MAX_VALUE  100000

namespace LIA
{
    class ObjLoader {
        public:
            static void load(
                ModelData&,
                const char * dir, 
                const char * path
            );
            static void load(
                ModelData&,
                std::string dir, 
                std::string path
            );
    };
} // namespace LIA
#endif