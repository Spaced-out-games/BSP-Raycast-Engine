#pragma once
#include <GL/glew.h>
#include <iostream>

template <GLenum Usage = GL_STATIC_DRAW, GLsizeiptr Size = 132>
class uBuffer {
public:
    uBuffer() : bufferID(0), bufferSize(0) {}

    ~uBuffer() {
        if (bufferID != 0) {
            glDeleteBuffers(1, &bufferID);
        }
    }

    void init() {
        if (bufferID != 0) {
            std::cerr << "Buffer already initialized." << std::endl;
            return;
        }

        bufferSize = Size;
        glGenBuffers(1, &bufferID);
        if (bufferID == 0) {
            std::cerr << "Failed to generate buffer." << std::endl;
            return;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
        glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, Usage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void bindBase(GLuint bindingPoint) const {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferID);
    }

    void updateData(GLintptr offset, GLsizeiptr size, const void* data) const {
        if (bufferID == 0) {
            std::cerr << "Buffer not initialized." << std::endl;
            return;
        }
        //std::cout << "Uniform Buffer updated.\n";

        glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    GLuint getID() const { return bufferID; }

private:
    GLuint bufferID;
    GLsizeiptr bufferSize;
};
