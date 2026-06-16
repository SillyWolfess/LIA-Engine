#include "loaders/materialLoader.hpp"
#include "tools/macros.hpp"
#include "tools.hpp"
#include "logs.hpp"

#include <sstream>
#include <glm/glm.hpp>
/***
 * Kd - color [done]
 * map_Kd - color texture [done]
 * 
 * Ka - ambient reflectivity [done]
 * 
 * Ni - optical density - index of refraction (glass)
 * 
 * map_Bump - bump texture [done]
 * 
 * Ns - shininess exponent [done]
 * map_Ns - shininess texture
 * 
 * Ks - Specular Color [done]
 * map_Ks - specular texture 
 * 
 * Pr - Roughness
 * map_Pr - roughness texture
 * 
 * Ke - emissive color
 * map_Ke - emissive texture [done]
 */
bool LIA::MaterialLoader::loadLib(MaterialLib& materialLib) {
	LIA_TRY
		LIA_trace_f("Loading lib {}", materialLib._name);
		std::string folder = sanitizeFolderPath(materialLib._folder);
		LIA_trace_f("Sanititzed folder '{}'", folder);
		std::string ext = {".mtl"};
		std::string fileName = sanitizeFileName(materialLib._name, ext);
		LIA_trace_f("Sanitized file name '{}'", fileName);
		std::string path = std::vformat("{}{}", std::make_format_args(folder, fileName));
	    LIA_trace_f("Target file path '{}'", path);

		FILE *file = fopen(path.c_str(), "r");
        if (file == nullptr) {
            LIA_error_f("Failed to find material file {} on the path {}", materialLib._name, path);
            return false;
        }
        
		LIA_trace_f("File for {} opened", materialLib._name);
        char lineHeader[128];
        bool loading = false;
		LIA::Material mTmp;
		std::string lastName = "";
        while (!feof(file)) {
			LIA::Material &material = lastName == "" ? mTmp : materialLib._materials[lastName];
            fscanf(file, "%s", lineHeader);
            if (strcmp(lineHeader, "newmtl") == 0) {
                char text[255];
                fscanf(file, "%s\n", &text);
                loading = true;
				Material tmp;
				std::string name = {text};
				materialLib._materials.emplace(std::pair<std::string, Material>(name, tmp));
				Material &m = materialLib._materials[name];
				m.name = name;
				m.folder = materialLib._folder;
				m.path = path;
				lastName = name;
            }
            else if (!loading) {
                char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
            }
            else if (strcmp(lineHeader,"Ns") == 0) {
				float read = 0;
				fscanf(file, " %f\n", &read);
				material.Ns = read;
			}
			else if (strcmp(lineHeader,"Ka") == 0) {
				glm::vec3 read;
				fscanf(file, " %f %f %f\n", &read.x, &read.y, &read.z);
				material.Ka[0] = read.x;
				material.Ka[1] = read.y;
				material.Ka[2] = read.z;
			}
			else if (strcmp(lineHeader,"Kd") == 0) {
				glm::vec3 read;
				fscanf(file, " %f %f %f\n", &read.x, &read.y, &read.z);
				material.Kd[0] = read.x;
				material.Kd[1] = read.y;
				material.Kd[2] = read.z;
			}
			else if(strcmp(lineHeader,"Ks") == 0) {
				float x,y,z;
				fscanf(file, " %f %f %f\n", &x, &y, &z);
				material.Ks[0] = x;
				material.Ks[1] = y;
				material.Ks[2] = z;
			}
            else if(strcmp(lineHeader,"Ke") == 0) {
				float x,y,z;
				fscanf(file, " %f %f %f\n", &x, &y, &z);
				material.Ke[0] = x;
				material.Ke[1] = y;
				material.Ke[2] = z;
			}
			else if(strcmp(lineHeader,"Ni") == 0) {
				float read=0;
				fscanf(file, " %f\n", &read);
				material.Ni = read;
			}
			else if(strcmp(lineHeader,"illum") == 0) {
				int read=0;
				fscanf(file, " %d\n", &read);
				material.illum = read;
			}
			else if(strcmp(lineHeader,"map_Kd") == 0) {
				char text[255];
				fscanf(file, "%s\n", &text);
				material.texture = {text};
				material.hasTexture = true;
			}
			else if(strcmp(lineHeader,"map_Ke") == 0) {
				char text[255];
				fscanf(file, "%s\n", &text);
				material.emTexture = {text};
				material.hasEm = true;
			}
			else if (strcmp(lineHeader,"map_Bump") == 0) {
				char text[255];
				fscanf(file," -bm 1.000000 %s\n", &text);
				material.bumpTexture = {text};
				material.hasBump = true;
			}
			else
			{
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}
        };
		materialLib._loaded = true;
		return true;
	LIA_CATCH_RETURN_FALSE
};

bool LIA::MaterialLoader::loadFromLib(Material& material) {
    LIA_TRY
        std::string folder = material.folder;
        std::string path = material.path;
        std::string name = material.name;

        FILE *file = fopen(path.c_str(), "r");
        if (file == nullptr) {
            LIA_error_f("Failed to find material file {} on the path {}", name, path);
            return false;
        }
        
        char lineHeader[128];
        bool loading = false;
        while (!feof(file)) {
            fscanf(file, "%s", lineHeader);
            if (strcmp(lineHeader, "newtml") == 0) {
                char text[255];
                fscanf(file, "%s\n", &text);
                loading = strcmp(text, name.c_str()) == 0;
            }
            else if (!loading) {
                char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
            }
            else if (strcmp(lineHeader,"Ns") == 0) {
				float read = 0;
				fscanf(file, " %f\n", &read);
				material.Ns = read;
			}
			else if (strcmp(lineHeader,"Ka") == 0) {
				glm::vec3 read;
				fscanf(file, " %f %f %f\n", &read.x, &read.y, &read.z);
				material.Ka[0] = read.x;
				material.Ka[1] = read.y;
				material.Ka[2] = read.z;
			}
			else if (strcmp(lineHeader,"Kd") == 0) {
				glm::vec3 read;
				fscanf(file, " %f %f %f\n", &read.x, &read.y, &read.z);
				material.Kd[0] = read.x;
				material.Kd[1] = read.y;
				material.Kd[2] = read.z;
			}
			else if(strcmp(lineHeader,"Ks") == 0) {
				float x,y,z;
				fscanf(file, " %f %f %f\n", &x, &y, &z);
				material.Ks[0] = x;
				material.Ks[1] = y;
				material.Ks[2] = z;
			}
            else if(strcmp(lineHeader,"Ke") == 0) {
				float x,y,z;
				fscanf(file, " %f %f %f\n", &x, &y, &z);
				material.Ke[0] = x;
				material.Ke[1] = y;
				material.Ke[2] = z;
			}
			else if(strcmp(lineHeader,"Ni") == 0) {
				float read=0;
				fscanf(file, " %f\n", &read);
				material.Ni = read;
			}
			else if(strcmp(lineHeader,"illum") == 0) {
				int read=0;
				fscanf(file, " %d\n", &read);
				material.illum = read;
			}
			else if(strcmp(lineHeader,"map_Kd") == 0) {
				char text[255];
				fscanf(file, "%s\n", &text);
				material.texture = {text};
				material.hasTexture = true;
			}
			else if(strcmp(lineHeader,"map_Ke") == 0) {
				char text[255];
				fscanf(file, "%s\n", &text);
				material.emTexture = {text};
				material.hasEm = true;
			}
			else if (strcmp(lineHeader,"map_Bump") == 0) {
				char text[255];
				fscanf(file," -bm 1.000000 %s\n", &text);
				material.bumpTexture = {text};
				material.hasBump = true;
			}
			else
			{
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}
        }; 
        return true;
    LIA_CATCH_RETURN_FALSE
}
