#ifndef LIA_MODEL_MANAGER
#define LIA_MODEL_MANAGER
#include <vector>
#include <string>
#include <map>
#include "./../data/Model.hpp"
#include "./../manager/shaderManager.hpp"

namespace LIA {
    class ModelManager {
        private:
            std::map<std::string, int> _nameToModel;
            std::vector<Model> _models;        
            bool catchGlError();    
            Model* create();
            void computeTangentBasis(
                // inputs
                std::vector<glm::vec3> & vertices,
                std::vector<glm::vec2> & uvs,
                // outputs
                std::vector<glm::vec3> & tangents,
                std::vector<glm::vec3> & bitangents
            );
            std::map<std::string, std::string> _paths;
            std::string getPath(std::string);
        public:
            /***
            * @param {string} name/id
            * @param {string} path 
            */
            bool registerPath(std::string, std::string);
            bool registerPathsFromFile(std::string);
            Model* create(std::string);
            bool load(ShaderManager*);
            bool load(Model&, int);
            bool loadModel(int);
            Model* get(int indx);
            /*
            Model* getByPath(std::string path);
            */
            Model* getByName(std::string);
            int find(std::string path);
            int size();
            void logNameToModelMap();
            void logModels();
    };
}
#endif