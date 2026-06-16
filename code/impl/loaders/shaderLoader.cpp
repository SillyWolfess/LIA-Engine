#include "loaders/shaderLoader.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

static char* LIA::textFileRead(const char *fileName) {
	char* text;
	
	if (fileName != NULL) {
		FILE *file = fopen(fileName, "rt");
	
		if (file != NULL) {
			fseek(file, 0, SEEK_END);
			int count = ftell(file);
			rewind(file);
			
			if (count > 0) {
				text = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(text, sizeof(char), count, file);
				text[count] = '\0';
			}
			fclose(file);
		}
	}
	return text;
}

bool LIA::loadShaders(GLuint& programID,const char * vertex_file_path,const char * fragment_file_path,  const char* geom_file_path, bool use_geom) {
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint GeomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    
    // Read the Vertex Shader code from the file
    
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        std::cout<<vertex_file_path<<" is open \n";
        while(std::getline(VertexShaderStream, Line))
        {
            VertexShaderCode += "\n" + Line;
        }
        VertexShaderStream.close();
    }
    else
    {
        std::cout << "I cannot open the file " << vertex_file_path<<std::endl;
        return false;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open())
    {
        std::string Line = "";
        std::cout<<fragment_file_path<<" is open \n";
        while(getline(FragmentShaderStream, Line))
        {
            FragmentShaderCode += "\n" + Line;
        }
        FragmentShaderStream.close();
    }
    else
    {
        std::cout << "I cannot open the file " << fragment_file_path<<std::endl;
        return false;
    }
    
    GLint Result  = GL_FALSE;
    int InfoLogLength;
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(Result == GL_FALSE)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
        glDeleteShader(VertexShaderID);
        return false;
    }
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(Result == GL_FALSE)
    {
        
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
        glDeleteShader(FragmentShaderID);
        return false;
    }

     if (use_geom) {
        std::cout << "using geom shader" << std::endl;
        // Read the Geometry Shader code from the file
        std::string GeomShaderCode;
        std::ifstream GeomShaderStream( geom_file_path, std::ios::in);
        if(GeomShaderStream.is_open())
        {
            std::string Line = "";
            std::cout<<geom_file_path<<" is open \n";
            while(getline(GeomShaderStream, Line))
            {
            GeomShaderCode += "\n" + Line;
            }
            GeomShaderStream.close();
        }
        else
        {
            std::cout << "I cannot open the file " << geom_file_path<<std::endl;
            return false;
        }

        // Compile Geometry Shader
        printf("Compiling shader : %s\n", geom_file_path);
        char const * GeomSourcePointer = GeomShaderCode.c_str();
        glShaderSource(GeomShaderID, 1, &GeomSourcePointer , NULL);
        glCompileShader(GeomShaderID);
        
        // Check Geom Shader
        glGetShaderiv(GeomShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(GeomShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(Result == GL_FALSE)
        {
            std::vector<char> GeomShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(GeomShaderID, InfoLogLength, NULL, &GeomShaderErrorMessage[0]);
            fprintf(stdout, "%s\n", &GeomShaderErrorMessage[0]);
            glDeleteShader(GeomShaderID);
            return false;
        }
    }
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    if (use_geom) {
        glAttachShader(ProgramID, GeomShaderID);
    }
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage(glm::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    
    if(Result == GL_FALSE)
    {
        glDeleteProgram(ProgramID);
        return false;
    }
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    glDeleteShader(GeomShaderID);
    
    programID = ProgramID;
    printf("shader created\n");
    return true;
}