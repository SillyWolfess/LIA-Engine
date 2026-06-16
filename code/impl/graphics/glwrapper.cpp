#include "graphics/glwrapper.hpp"

bool LIA::catchGlError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        char error[500];
        sprintf(error,"GL ERROR: code %d", err);
        LIA_error(error);
        return true;
    }
    while(glGetError() != GL_NO_ERROR){ };
    return false;
}

bool LIA::getUniform(GLint &out, GLuint shader_program, const char* name) {
    out = glGetUniformLocation(shader_program, name);
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        char error[500];
        sprintf(error,"GL ERROR: code %d: cannot get %s from %d", err, name, shader_program);
        LIA_error(error);
        return false;
    }
    return true;
}
/*
void LIA::set3fv(GLuint shader_program, const char* name, float v1, float v2, float v3) {
    glm::vec3 v(v1, v2, v3);
    glUniform3fv(glGetUniformLocation(shader_program, name), 1, &v[0]);
}

void LIA::setFloat(GLuint shader_program, const char* name, float v) {
    glUniform1f(glGetUniformLocation(shader_program, name), v);
}
*/