#ifndef LIA_GRAPHIC_OBJECT
#define LIA_GRAPHIC_OBJECT

#include <vector>
#include <string>
#include <map>
#include <set>

#include "./../precompiled.hpp"

#define LIA_GRAPHIC_OBJECT_DEBUG 0
namespace LIA {
    struct gMaterial {
        char name[255];
        char texture[255];
        char texture_bump[255];
        char texture_em[255];
        double Ns;
        double Ka[3];
        double Kd[3];
        double Ks[3];
        double Ke[3];
        double Ni;
        double d;
        int illum;

        int textureIndex;
        bool hasTexture = false;
        bool hasBump = false;
        bool hasEm = false;
/*
        GLuint textureID = 0;
        GLuint bumpID = 0;
        GLuint emID = 0;
*/
        #if LIA_DEBUG_ON && LIA_GRAPHIC_OBJECT_DEBUG
        gMaterial();
        gMaterial(const gMaterial&);
        ~gMaterial();
        #endif
    };
    struct ModelData
    {
        bool _complete;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> colours;
        std::vector<glm::vec3> normals;
            
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        std::vector<gMaterial> materials;
        std::vector<int> materialIds;
        std::vector<int> offsets;

        float min_x;
        float min_y;
        float min_z;
        float max_x;
        float max_y;
        float max_z;

        #if LIA_DEBUG_ON && LIA_GRAPHIC_OBJECT_DEBUG
        ModelData();
        ModelData(const ModelData&);
        ~ModelData();
        #endif
    };
}
#endif