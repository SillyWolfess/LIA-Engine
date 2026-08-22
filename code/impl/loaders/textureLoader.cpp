#include "loaders/textureLoader.hpp"
#include "graphics/glwrapper.hpp"

#include <string>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "3d-party/stb_image.h"

bool LIA::TextureLoader::loadBMP(GLuint &textureID_out, std::string imagepath) {
	stbi_set_flip_vertically_on_load(true);
	LIA_TRY
		const bool debug_texture = false;
		LIA_debug_f("Reading bmp image {}", imagepath);
		// Data read from the header of the BMP file
		unsigned char header[54];
		unsigned int dataPos;
		unsigned int imageSize;
		unsigned int width, height;
		// Actual RGB data
		unsigned char * data;

		// Open the file
		FILE * file = fopen(imagepath.c_str(), "rb");
		if (!file)							    
		{
			LIA_fatal_f("{} not found", imagepath);
			return false;
		}

		// If less than 54 bytes are read, problem
		if ( fread(header, 1, 54, file)!=54 ){ 
			LIA_fatal("Not a correct BMP file: less than 54 bytes");
			fclose(file);
			return false;
		}
		// A BMP files always begins with "BM"
		if ( header[0]!='B' || header[1]!='M' ){
			LIA_fatal("Not a correct BMP file: missing BM");
			fclose(file);
			return false;
		}
		// Make sure this is a 24bpp file
		if ( *(int*)&(header[0x1E])!=0  ) { 
			LIA_fatal("Not a correct BMP file: not 24bpp file"); 
			fclose(file);
			return false;
		}
		if ( *(int*)&(header[0x1C])!=24 ) {
			LIA_fatal("Not a correct BMP file: not 24bpp file"); 
			fclose(file); 
			return false;
		}

		// Read the information about the image
		dataPos    = *(int*)&(header[0x0A]);
		imageSize  = *(int*)&(header[0x22]);
		width      = *(int*)&(header[0x12]);
		height     = *(int*)&(header[0x16]);

		// Some BMP files are misformatted, guess missing information
		if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
		if (dataPos==0)      dataPos=54; // The BMP header is done that way

		// Create a buffer
		data = new unsigned char [imageSize];

		// Read the actual data from the file into the buffer
		fread(data,1,imageSize,file);

		// Everything is in memory now, the file wan be closed
		fclose (file);
		GLuint textureID;
		// Create one OpenGL texture
		glGenTextures(1, &textureID);
		
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		// OpenGL has now copied the data. Free our own version
		delete [] data;
		fclose(file);

		// Poor filtering, or ...
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

		// Return the ID of the texture we just created
		textureID_out = textureID;
		LIA_debug_f("new texture id: {}", textureID);
		return true;
	LIA_CATCH_RETURN_FALSE
}

bool LIA::TextureLoader::_loadTexture(GLuint &textureID_out, std::string imagepath, GLenum format) {
	stbi_set_flip_vertically_on_load(true);
	LIA_TRY
		LIA_debug_f("Reading image {}", imagepath);
		int width, height, nrChannels;
		unsigned char *data = stbi_load(imagepath.c_str(), &width, &height, &nrChannels, 0); 
		if (data) {
			GLuint textureID;
			// Create one OpenGL texture
			glGenTextures(1, &textureID);
			if (catchGlError()) {
				LIA_fatal_f("Failed to generate texture id");
				return false;
			}
			// "Bind" the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, textureID);
			if (catchGlError()) {
				LIA_fatal_f("Failed to bind texture {}" , textureID);
				return false;
			}
			// Give the image to OpenGL
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			if (catchGlError()) {
				LIA_fatal("Failed to send image to opengl");
				return false;
			}
			// Poor filtering, or ...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

			// Return the ID of the texture we just created
			stbi_image_free(data);
			textureID_out = textureID;
			LIA_debug_f("New texture id: {}", textureID);
			return true;
		} else {
			LIA_error_f("Failed to create texture from {}", imagepath);
			return false;
		}
	LIA_CATCH_RETURN_FALSE
}

bool LIA::TextureLoader::loadTextureRGBA(GLuint &textureID_out, std::string imagepath) {
    return _loadTexture(textureID_out, imagepath, GL_RGBA);
}

bool LIA::TextureLoader::loadTextureRGB(GLuint &textureID_out, std::string imagepath) {
	return _loadTexture(textureID_out, imagepath, GL_RGB);
}

bool LIA::TextureLoader::loadTexture(GLuint &textureID_out, std::string imagepath) {
	stbi_set_flip_vertically_on_load(true);
    LIA_debug_f("Reading image {}", imagepath);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(imagepath.c_str(), &width, &height, &nrChannels, 0); 
	if (data) {
		GLenum format;
		if (nrChannels == 1) {
			format = GL_RED;
            LIA_debug("Channel = RED");
		} else if (nrChannels == 2) {
            LIA_debug("Channel = RG");
			format = GL_RG;
		} else if (nrChannels == 3) {
            LIA_debug("Channel = RGB");
			format = GL_RGB;
		} else if (nrChannels == 2) {
            LIA_debug("Channel = RGBA");
			format = GL_RGBA;
		} else {
            LIA_fatal_f("Unsupported number of channels: {}", nrChannels);
			return false;
		}

		GLuint textureID;
		// Create one OpenGL texture
		glGenTextures(1, &textureID);
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		// Poor filtering, or ...
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

		// ... nice trilinear filtering.
		// Return the ID of the texture we just created
		stbi_image_free(data);
		textureID_out = textureID;
		LIA_debug_f("New texture id: {}", textureID);
		return true;
	} else {
        LIA_error_f("Failed to create texture from {}", imagepath);
		return false;
	}
}

bool LIA::TextureLoader::loadTextureHDR(GLuint &textureID_out, std::string imagepath) {
	stbi_set_flip_vertically_on_load(true);
    LIA_debug_f("Reading image HDR [RGB locked] {}", imagepath);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(imagepath.c_str(), &width, &height, &nrChannels, 0); 
	if (data) {
		GLenum format;
		if (nrChannels == 1) {
			format = GL_RED;
            LIA_debug("Channel = RED");
		} else if (nrChannels == 2) {
            LIA_debug("Channel = RG");
			format = GL_RG;
		} else if (nrChannels == 3) {
            LIA_debug("Channel = RGB");
			format = GL_RGB;
		} else if (nrChannels == 4) {
            LIA_debug("Channel = RGBA");
			format = GL_RGBA;
		} else {
            LIA_fatal_f("Unsupported number of channels: {}", nrChannels);
			return false;
		}

		GLuint textureID;
		// Create one OpenGL texture
		glGenTextures(1, &textureID);
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		// Poor filtering, or ...
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

		// ... nice trilinear filtering.
		// Return the ID of the texture we just created
		stbi_image_free(data);
		textureID_out = textureID;
		LIA_debug_f("New texture id: {}", textureID);
		return true;
	} else {
        LIA_error_f("Failed to create texture from {}", imagepath);
		return false;
	}
}