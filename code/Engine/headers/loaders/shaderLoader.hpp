#ifndef LIA_SHADER_LOADER_HPP
#define LIA_SHADER_LOADER_HPP
#include "./../precompiled.hpp"

namespace LIA {
    static char* textFileRead(const char *fileName);    
    bool loadShaders(GLuint& programID,const char * vertex_file_path,const char * fragment_file_path, const char* geom_shader_path, bool use_geom);
}
#endif