#pragma once
#include <GL/glew.h>
#include <iostream>
#include "stb_image.h" // Make sure to include stb_image.h for image 

template <GLenum TextureType = GL_TEXTURE_2D,
    GLint InternalFormat = GL_RGB,
    GLenum Format = GL_RGB,
    GLint WrapS = GL_REPEAT,
    GLint WrapT = GL_REPEAT,
    GLint MinFilter = GL_NEAREST,
    GLint MagFilter = GL_NEAREST>
class Texture {
public:
    GLuint TextureID;

    Texture() : TextureID(0) {
        // Constructor can be empty or include initialization logic if needed
    }

    // Load texture from file
    bool loadFromFile(const char* filePath) {
        glGenTextures(1, &TextureID);
        glBindTexture(TextureType, TextureID);

        // Set texture parameters
        glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, WrapS);
        glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, WrapT);
        glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, MinFilter);
        glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, MagFilter);

        GLfloat maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

        // Load image
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(TextureType, 0, InternalFormat, width, height, 0, Format, GL_UNSIGNED_BYTE, data);
            //glGenerateMipmap(TextureType);
        }
        else {
            std::cerr << "Failed to load texture: " << filePath << std::endl;
            stbi_image_free(data);
            return false;
        }

        stbi_image_free(data);
        return true;
    }

    // Bind texture
    void bind() const {
        glBindTexture(TextureType, TextureID);
    }

    // Unbind texture
    void unbind() const {
        glBindTexture(TextureType, 0);
    }

    // Destructor
    ~Texture() {
        if (TextureID != 0) {
            glDeleteTextures(1, &TextureID);
        }
    }
};