#ifndef LIA_OBJECT_MANAGER
#define LIA_OBJECT_MANAGER

#include <vector>
#include "./../data/Object.hpp"
#include "modelManager.hpp"

namespace LIA {
    class ShaderManager;
    class Scene;
    class ObjectManager {
        private:
            ModelManager _modelManager;
            std::map<std::string, XmlLoader::XmlData> _cache;
            std::map<std::string, XmlLoader::XmlData> _objectCache;
            std::map<std::string, XmlLoader::XmlData> _pathCache;
            std::map<std::string, std::string> _paths;
            std::map<std::string, std::string> _templatePaths;
            std::vector<Object> _objects;
            int create();
            std::string getPath(std::string);
            std::string getTemplatePath(std::string);
            bool loadMaterials(Object& object, Model* model);
        public:
            void reset();
            /***
            * @param {string} name/id
            * @param {string} path 
            */
            bool registerPath(std::string, std::string);
            /***
            * @param {string} name/id
            * @param {string} path 
            */
            bool registerTemplatePath(std::string, std::string);
            bool registerPathsFromFile(std::string);
            bool registerTemplatePathsFromFile(std::string);
            /***
             * @param {string} id
             * @param {string} custom identifier
             */
            bool createObject(std::string, std::string);

            int create(std::string);
            bool remove(std::vector<std::string> &toRemove);
            Object* get(int);
            int size();
            bool load(ShaderManager*);
            bool loadObject(std::string, std::string);
            bool loadModel(Object&);

            void pass(Scene*);
            void setModel(Object* object, std::string);

            void setPosition(Object* object, float x, float y, float z);
            void setPosition(Object*, Position&);

            void setRotation(Object* object, float x, float y, float z);
            void setRotation(Object* object, float rotation);
            void setRotation(Object*, Rotation&);

            void setScale(Object* object, float x, float y, float z);
            void setScale(Object* object, float scale);
            void setScale(Object*, Scale&);

            void setType(Object*, std::string);

            void print(Object* object);
            void print();

            int getPlayer(int);
            int getByName(int, std::string);
            ModelManager* getModelManager() { return &_modelManager; };
    };
}
#endif