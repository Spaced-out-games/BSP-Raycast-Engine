#pragma once
#include <GL/glew.h>
#include <iostream>
#include <cstddef>
#include "globals.h"


template <GLenum Usage = GL_STATIC_DRAW, typename T = gl_globals>
class uBuffer {
public:
    uBuffer() : bufferID(0) {
        std::memset(&data, 0, sizeof(T)); // Initialize to zero
    }

    ~uBuffer() {
        if (bufferID != 0) {
            glDeleteBuffers(1, &bufferID);
        }
    }

    // Initialize the buffer
    void init() {
        if (bufferID != 0) {
            std::cerr << "Buffer already initialized." << std::endl;
            return;
        }

        glGenBuffers(1, &bufferID);
        if (bufferID == 0) {
            std::cerr << "Failed to generate buffer." << std::endl;
            return;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, Usage);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // Bind the buffer to a binding point
    void bindBase(GLuint bindingPoint) const {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferID);
    }

    // Set a specific member using an offset
    template <typename MemberType>
    void set(size_t offset, const MemberType& newValue) {
        // Check if the offset is valid
        if (offset + sizeof(MemberType) > sizeof(T)) {
            std::cerr << "Offset out of bounds." << std::endl;
            return;
        }

        // Copy the new value to the specified offset
        std::memcpy(reinterpret_cast<char*>(&data) + offset, &newValue, sizeof(MemberType));
        update(); // Update the GPU buffer
    }

    // Get a specific member using an offset
    template <typename MemberType>
    MemberType get(size_t offset) {
        // Check if the offset is valid
        if (offset + sizeof(MemberType) > sizeof(T)) {
            std::cerr << "Offset out of bounds." << std::endl;
            return MemberType(); // Return default-constructed value
        }

        // Return the value at the specified offset
        return *reinterpret_cast<MemberType*>(reinterpret_cast<char*>(&data) + offset);
    }

    // Update the buffer data on the GPU
    void update() const {
        if (bufferID == 0) {
            std::cerr << "Buffer not initialized." << std::endl;
            return;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

private:
    GLuint bufferID; // OpenGL buffer ID
    T data; // Local copy of data
};