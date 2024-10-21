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


void showOpenGLState()
{
    GLint currentVAO = 0, currentVBO = 0, currentEBO = 0, currentShader = 0;

    // Get the currently bound VAO, VBO, EBO, and shader program
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);

    // Print the state values
    std::cout << "Currently bound VAO: " << currentVAO << std::endl;
    std::cout << "Currently bound VBO: " << currentVBO << std::endl;
    std::cout << "Currently bound EBO: " << currentEBO << std::endl;
    std::cout << "Currently bound Shader Program: " << currentShader << std::endl;

    // Additional state queries can go here, such as texture bindings, framebuffer bindings, etc.

    // Example for querying the currently active texture unit
    GLint activeTexture = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    std::cout << "Active Texture Unit: " << activeTexture - GL_TEXTURE0 << std::endl;

    // Query current frame buffer
    GLint currentFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
    std::cout << "Currently bound Framebuffer: " << currentFBO << std::endl;

    // Query current viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    std::cout << "Viewport: " << viewport[0] << ", " << viewport[1] << ", "
        << viewport[2] << ", " << viewport[3] << std::endl;

    // Optionally, you can extend it to query and display other OpenGL states if needed
}