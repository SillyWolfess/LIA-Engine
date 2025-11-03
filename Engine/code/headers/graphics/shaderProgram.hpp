#ifndef LIA_SHADER_PROGRAM_HPP
#define LIA_SHADER_PROGRAM_HPP
#include "precompiled.hpp"
#include "data/tShader.hpp"
#include "data/ModelData.hpp"
#include "data/Light.hpp"
#include <map>
#include <unordered_map>
#include "watcher/Watcher.hpp"

namespace LIA {
    class ShaderProgram {
        private:
            Watcher _watcher;
            struct sShaderReturn {
                GLuint id;
                bool success;
            };
            GLuint _program;

            GLint _mvpId;
            GLint _vpId;
            GLint _mId;
            GLint _colorId;

            std::unordered_map<std::string, GLint> _uniforms;
            bool _loaded;
            std::string _name;
            std::map<LIA_SHADER, std::string> _shaderPaths;
            
            bool buildProgram();
            sShaderReturn createShader(const char*, GLenum);
            sShaderReturn createShader(std::string, GLenum);
            std::string getShaderPath(LIA_SHADER);
            void setVec3(GLuint shader_program, const char* name, float v1, float v2, float v3);
            void setFloat(GLuint shader_program, const char* name, float v);
        public:
            ~ShaderProgram();
            ShaderProgram();
            ShaderProgram(const ShaderProgram&);
            ShaderProgram(std::string);
            /***
             * Add shader path
             * @param {LIA_SHADER} shader type
             * @param {std::string} path to shader
             */
            void add(LIA_SHADER, std::string);
            void update();
            /***
             * Builds shader in GPU
             */
            bool load();
            bool bind();
            bool getUniform(GLint&, std::string);
            bool sendToGPU(GLint id, int count, GLboolean transpose, const GLfloat *value);
            bool sendToGPU(GLint id, float v1, float v2, float v3, float v4);

            bool loadUniforms();
            
            bool sendMatrix(GLint id, GLfloat* value);
            bool sendMVP(GLfloat *value);
            bool sendM(GLfloat *value);
            bool sendVP(GLfloat *value);
            bool sendColor(Color&);
            bool sendMaterial(gMaterial&);
            bool sendLight(Light& light);
            /***
             * string - uniform name
             * int - uniform id
             * int - texture id
             * string - texture name
             */
            bool bindTexture(std::string, int, int, std::string);
            /***
             * string - uniform name
             * int  - uniform id
             */
            bool unbindTexture(std::string, int);

            std::string getName() { return _name; }
            GLuint getProgram() { return _program; }
            /*
            void destroy();
            */
    };
}
#endif