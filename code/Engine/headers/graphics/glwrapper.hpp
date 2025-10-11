#ifndef GL_WRAPPER_HPP
#define GL_WRAPPER_HPP
#include "precompiled.hpp"

namespace LIA {
    bool catchGlError();
    bool getUniform(GLint &out, GLuint shader_program, const char* name);
    /*
    void setFloat(GLuint shader_program, const char* name, float v);
    void set3fv(GLuint shader_program, const char* name, float v1, float v2, float v3);
    */
}
    
#endif