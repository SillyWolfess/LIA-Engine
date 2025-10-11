#ifndef LIA_SCENE
#define LIA_SCENE

#include "precompiled.hpp"
#include "camera.hpp"
#include "data/position.hpp"
#include "data/sceneObject.hpp"
#include "manager/shaderManager.hpp"
#include "data/Texture.hpp"
#include "data/Light.hpp"
#include "data/ModelData.hpp"

#include <vector>

namespace LIA {
    class Scene {
        private:
            bool _isWireMode;
            /**
             * r, g, b, a
             */
            struct s_background {
                int r, g, b, a;
            } background;

            /**
             * string _shader
             * GLuint _vao
             * bool _useIndices
             * bool _passColor
             * int _size
             * bool _loaded
             */
            struct Prefab {
                std::string _shader;
                GLuint _vao;
                bool _useIndices;
                bool _passColor;
                int _size;

                bool _loaded = false;
            };

            std::vector<SceneObject> _storage;
            std::map<std::string, int> _storageMap;
            std::map<std::string, Prefab> _prefabs;
//            std::vector<SceneObject> _objects;
            std::vector<Light> _lightSources;

            std::map<std::string, std::vector<SceneObject>> _shaderObjectMap;
/*
            struct ShaderData {
                GLuint program;
            };
*/
/*
            bool drawObject(glm::mat4& VP, SceneObject&, ShaderManager*);
*/
            /*
            ShaderData setShaderProgram(SceneObject& object, ShaderManager*);
            */
            bool drawVAO(SceneObject& object, ShaderProgram& /* ShaderData& shaderData*/, glm::mat4& VP);

            bool makePrefab(GLuint, Model&, const char*);
            bool prefabSquare(ShaderManager&);
            bool prefabCube(ShaderManager&);
            SceneObject& addToShaderMap(std::string);
            bool prepareShader(ShaderProgram& shader, glm::mat4& VP);
            bool sendLightSource(ShaderProgram&);
        public:
            bool init();
            SceneObject* get(int, std::string);
            bool initPrefab(ShaderManager&);
            bool draw(Camera&, ShaderManager*);
//            void destroy();

            /**
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * GLuint - vao
             * std::string - shader
             * bool - hasIndices
             * int - number of verteces/indices
             */
            bool add( Position, Rotation, Scale, GLuint, std::string, bool, int);
            /**
             * string - identifier
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * GLuint - vao
             * std::string - shader
             * bool - hasIndices
             * int - number of verteces/indices
             * materials
             * material ids
             * textures
             * bump textures
             * em textures
             * int - offset
             */
            bool add(std::string, Position, Rotation, Scale, GLuint, std::string, bool, int,
                std::vector<LIA::gMaterial>&, std::vector<int>&,
                std::vector<Texture>&, std::vector<Texture>&, std::vector<Texture>&,
                int
            );
            /**
             * string - identifier
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * GLuint - vao
             * std::string - shader
             * bool - hasIndices
             * int - number of verteces/indices
             * materials
             * material ids
             * textures
             * bump textures
             * em textures
             * int - offset
             * bool - draw as points
             */
            bool add(std::string, Position, Rotation, Scale, GLuint, std::string, bool, int,
                std::vector<LIA::gMaterial>&, std::vector<int>&,
                std::vector<Texture>&, std::vector<Texture>&, std::vector<Texture>&,
                int, bool
            );
            /**
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * Color - color
             */
            bool addSquare(Position, Rotation, Scale, Color);
            /**
             * string - identifier
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * Color - color
             */
            bool addSquare(std::string, Position, Rotation, Scale, Color);
            /**
             * SceneObject - object information to render
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * Color - color
             */
            bool addSquare(SceneObject&, Position, Rotation, Scale, Color);
            bool addCube(Position, Scale, Color);
            /**
             * Build objectInfo from prefab
             * SceneObject - target
             * string - prefab name
             */
            bool objectFromPrefab(SceneObject&, std::string);
            /**
             * SceneObject - object information to render
             */
            /*
            bool add(SceneObject&);
            */
            /**
             * Adds light source to the scene
             */
            int addLightSource();
            Light& getLight(int id) { return _lightSources[id]; };
            SceneObject& getFromStorage(std::string);
            void clear();
            Scene();
            ~Scene();
    };
}
#endif