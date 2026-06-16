#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <map>
#include <vector>
#include "./../graphics/shaderProgram.hpp"

namespace LIA {
    class ShaderManager {
        private:
            std::vector<ShaderProgram> _shaders;
            std::map<std::string, int> _shaderMap;
            bool validate(ShaderProgram&);
        public:
            /***
             * Get shader program Id
             * 
             * @param {std::string} program/shader name
             */
            GLuint getProgramId(std::string);
            ShaderProgram& getShader(std::string);
            bool init();
            void update();
            ~ShaderManager();
    };
}
#endif