#pragma once
#include <GL/glew.h>
#include <iostream>

#include "debug_utils.h"


#include <GL/glew.h>
#include <iostream>

/**
 * @brief A class to manage Vertex Buffer Objects (VBOs) for OpenGL.
 *
 * This template class simplifies the creation, binding, and management of VBOs.
 * The user specifies the type of vertex data and the drawing type (e.g., GL_STATIC_DRAW).
 *
 * @tparam T The type of the vertex data (e.g., glm::vec2, glm::vec3).
 * @tparam DrawingType The type of drawing (e.g., GL_STATIC_DRAW, GL_DYNAMIC_DRAW).
 */
template <typename T, GLenum DrawingType>
class vBuffer {
public:
    GLuint VBO; ///< The OpenGL handle for the vertex buffer object.

    /**
     * @brief Default constructor for vBuffer.
     * Initializes VBO to 0.
     */
    vBuffer() : VBO(0) {}

    /**
     * @brief Initializes the VBO with the given vertex data.
     *
     * Generates and binds a new VBO, uploads the vertex data to the GPU, and unbinds the VBO.
     *
     * @param vertices Pointer to the vertex data.
     * @param vertex_count Number of vertices in the vertex data.
     */
    void init(const T* vertices, size_t vertex_count) {
        glGenBuffers(1, &VBO); // Generate a new VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the VBO
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(T), vertices, DrawingType); // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO
    }

    /**
     * @brief Binds the VBO for use in OpenGL.
     *
     * This method should be called before setting up vertex attributes or issuing draw calls.
     */
    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    /**
     * @brief Unbinds the VBO.
     *
     * This method should be called after using the VBO to avoid unintended modifications.
     */
    void unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    /**
     * @brief Destructor for vBuffer.
     * Deletes the VBO if it has been created.
     */
    ~vBuffer() {
        
    }
    void destroy(){
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO); // Delete the VBO
        }
    }
    /**
     * @brief Checks if the VBO is currently bound.
     *
     * @return True if the VBO is currently bound, false otherwise.
     */
    bool isBound() const {
        GLint currentBuffer = 0;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer); // Get the currently bound VBO
        return (static_cast<GLuint>(currentBuffer) == VBO);
    }

    /**
     * @brief Gets the ID of the VBO.
     *
     * @return The OpenGL ID of the VBO.
     */
    GLuint getID() const {
        return VBO;
    }
};

