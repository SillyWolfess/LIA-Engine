#ifndef LIA_MATERIAL
#define LIA_MATERIAL

#include <string>

namespace LIA {
    struct Material {
        std::string name;
        std::string path;
        std::string folder;

        std::string texture;
        std::string bumpTexture;
        std::string emTexture;

        double Ns;
        double Ka[3];
        double Kd[3];
        double Ks[3];
        double Ke[3];
        double Ni;
        double d;
        int illum;

        bool hasTexture = false;
        bool hasBump = false;
        bool hasEm = false;

        bool isLoaded = false;
    };
}
#endif