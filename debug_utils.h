#pragma once
#include <GL/glew.h>
#include <iostream>

const char* getGLErrorString(GLenum errorCode) {
    switch (errorCode) {
    case GL_NO_ERROR:          return "No error";
    case GL_INVALID_ENUM:      return "Invalid enum";
    case GL_INVALID_VALUE:     return "Invalid value";
    case GL_INVALID_OPERATION: return "Invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
    case GL_OUT_OF_MEMORY:     return "Out of memory";
    case GL_STACK_UNDERFLOW:   return "Stack underflow";
    case GL_STACK_OVERFLOW:    return "Stack overflow";
    default:                  return "Unknown error";
    }
}

void checkGLError(const char* function, int line) {
    GLenum error = glGetError();
    while (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: '" << getGLErrorString(error) <<"' in " << function << " at line " << line << ": " << error << std::endl;
        error = glGetError();
    }
}

#define CHECK_GL_ERROR() checkGLError(__FUNCTION__, __LINE__)

void print_vec2(glm::vec2& vector, bool newline)
{
    std::cout << "(" << vector.x << ", " << vector.y << ")";
    if (newline)
    {
        std::cout << '\n';
    }
}

void print_vec3(glm::vec3& vector, bool newline)
{
    std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    if (newline)
    {
        std::cout << '\n';
    }
}


#define runtime_assert(expression, message) \
    if (!(expression)) { \
        std::cout << "Assertion failed: (" << #expression << "), message: " << message << std::endl; \
        std::exit(EXIT_FAILURE); \
    }
