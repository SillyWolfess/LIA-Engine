#ifndef LIA_TEXTURE_LOADER
#define LIA_TEXTURE_LOADER
#include "./../precompiled.hpp"
namespace LIA {
    class TextureLoader {
        public:
            static bool loadBMP(GLuint &textureID_out, std::string imagepath);
	    	static bool _loadTexture(GLuint &textureID_out, std::string imagepath, GLenum format);
	    	static bool loadTexture(GLuint &textureID_out, std::string imagepath);
	    	static bool loadTextureRGBA(GLuint &textureID_out, std::string imagepath);
		    static bool loadTextureRGB(GLuint &textureID_out, std::string imagepath);
		    static bool loadTextureHDR(GLuint &textureID_out, std::string imagepath);
    };
}
#endif