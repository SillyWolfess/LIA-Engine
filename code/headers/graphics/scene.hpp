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
#include "manager/materialManager.hpp"
#include "manager/textureManager.hpp"
#include "graphics/font.hpp"

#include <vector>

namespace LIA {
    class Scene {
        private:
            bool _isWireMode;
            bool _depthTest;
            bool _enableLogs;
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
            std::vector<Light> _lightSources;

            std::map<std::string, std::vector<SceneObject>> _shaderObjectMap;
            bool drawVAO(SceneObject& object, ShaderProgram&, glm::mat4& VP);

            bool makePrefab(GLuint, Model&, const char*, bool hasUvs = false);
            bool prefabSquare(ShaderManager&);
            bool prefabSprite(ShaderManager&);
            bool prefabCube(ShaderManager&);
            SceneObject& addToShaderMap(std::string);
            bool prepareShader(ShaderProgram& shader, glm::mat4& VP);
            bool sendLightSource(ShaderProgram&);
            bool useMaterial(MaterialManager& materialManager,ShaderProgram& shader,std::string materialLib, std::string materialName);
            bool useTexture(TextureManager& textureManager, ShaderProgram& shader, std::string textureName);
        public:
            bool init();
            void setDepthTest(bool value) { _depthTest = value; }
            SceneObject* get(int, int, std::string);
            bool initPrefab(ShaderManager&);
            bool draw(Camera&, ShaderManager*, Font*);
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
             * material lib
             * material name
             * int - offset
             */
            bool add(
                std::string,
                Position,
                Rotation,
                Scale,
                GLuint,
                std::string,
                bool,
                int,
                std::string,
                std::string,
                /*
                std::vector<LIA::gMaterial>&,
                std::vector<int>&,
                std::vector<Texture>&,
                std::vector<Texture>&,
                std::vector<Texture>&,
                */
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
             * material lib
             * material name
             * int - offset
             * bool - draw as points
             */
            bool add(
                std::string,
                Position,
                Rotation,
                Scale,
                GLuint,
                std::string,
                bool,
                int,
                std::string,
                std::string,
                int,
                bool
            );
            /**
             * string - identifier
             * Position - position
             * Rotation - rotation
             * Scale - scale
             * Color - color
             * string - texture
            */
            bool addSprite(std::string, Position, Rotation, Scale, Color, std::string);
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
             * Adds light source to the scene
             */
            int addLightSource();
            Light& getLight(int id) { return _lightSources[id]; };
            SceneObject& getFromStorage(std::string);
            void clear();
            Scene();
            ~Scene();
            // TEST
            struct Data {
                std::string text;
                Position position;
                Color color;
                int size;
            };
            struct Layer {
                std::vector<Data> _text;
                std::map<std::string, std::vector<SceneObject>> _shaderObjectMap;
            };
            int _layerId;
            std::map<int, Layer> _layerMap;
        //    std::vector<Data> _data;
            void addText(Font*, std::string, Position);
            void addText(Font*, std::string, Position, int);
            void addText(Font*, std::string, Position, Color);
            void addText(Font*, std::string, Position, Color, int);
            void setLayerId(int id) { _layerId = id; };
            void increaseLayerId() { _layerId++; };
            void clearText();

            void enableLogs(bool value) { _enableLogs = value; }
    };
}
#endif