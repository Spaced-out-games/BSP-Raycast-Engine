
/**
To use Mesh:
1. Set up SDL and OpenGL context.
2. Create a shader either from a file or from source code. //
3. Prepare your vertex data, which can involve loading from a file or creating in memory.
4. Pass the vertex data and shader to the Mesh3D constructor. The geometry will be set up at this point.
To draw:
5. Use the shader associated with this Mesh3D by calling <see cref="glUseProgram"/> with the shader's program ID.
6. Set any required uniforms.
7. Call draw() to render the mesh.
*/



#pragma once
#include <GL/glew.h>
#include <iostream>
#include "vArray.h"
#include "vBuffer.h"
#include "eBuffer.h"


#include <cstddef> // For offsetof
#include <GL/glew.h>
#include <iostream>
#include "vBuffer.h" // Assuming vBuffer is in a header file
#include "eBuffer.h" // Assuming eBuffer is in a header file
#include "vArray.h"  // Assuming vArray is in a header file

template <typename vertex_t, GLenum DrawingType, GLenum PrimitiveType>
class Mesh {
public:
    vArray VAO; ///< Vertex Array Object to store the vertex attributes and buffer bindings.
    vBuffer<vertex_t, DrawingType> VBO; ///< Vertex Buffer Object to store vertex data.
    eBuffer<DrawingType, PrimitiveType> EBO; ///< Element Buffer Object to store index data.
    #ifdef _DEBUG
    bool DrawLogged = 0;
    #endif // _DEBUG

    Mesh() {}

    Mesh(const vertex_t* vertices, size_t vertex_count, const GLuint* indices, size_t index_count)
        : VBO(), EBO() {
        init(vertices, vertex_count, indices, index_count);
    }

    ~Mesh() {
        // Cleanup handled by vArray destructor
    }

    void destroy() {
        VAO.destroy();
        VBO.destroy();
        EBO.destroy();
    }

    void init(const vertex_t* vertices, size_t vertex_count, const GLuint* indices, size_t index_count) {
        #ifdef _DEBUG
        //std::cout << "4. Pass the vertex data and shader to the Mesh constructor. The geometry will be set up at this point\n";
        #endif

        VAO.init();
        VAO.bind();

        VBO.init(vertices, vertex_count);
        VBO.bind();

        EBO.init(indices, index_count);
        EBO.bind();
    }

    void bind() {
        VAO.bind();
        VBO.bind();
        EBO.bind();
    }

    // Existing vec2, vec3, vec4 methods for floats
    void addVec2(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec3(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec4(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    // New methods for integer types
    void addInt8(GLuint index, std::size_t offset, bool normalized = false) {
        glVertexAttribIPointer(index, 1, GL_BYTE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addUInt8(GLuint index, std::size_t offset, bool normalized = false) {
        glVertexAttribIPointer(index, 1, GL_UNSIGNED_BYTE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addInt16(GLuint index, std::size_t offset) {
        glVertexAttribIPointer(index, 1, GL_SHORT, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addUInt16(GLuint index, std::size_t offset) {
        glVertexAttribIPointer(index, 1, GL_UNSIGNED_SHORT, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addInt32(GLuint index, std::size_t offset) {
        glVertexAttribIPointer(index, 1, GL_INT, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addUInt32(GLuint index, std::size_t offset) {
        glVertexAttribIPointer(index, 1, GL_UNSIGNED_INT, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    // OpenGL doesn't support 64-bit integers for vertex attributes, but we can emulate with two 32-bit integers
    void addInt64(GLuint index, std::size_t offset) {
        addUInt64(index, offset); // Can be implemented similarly
    }

    void addUInt64(GLuint index, std::size_t offset) {
        // OpenGL does not support native 64-bit integers for attributes, can be split into two 32-bit
        glVertexAttribIPointer(index, 2, GL_UNSIGNED_INT, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    // New methods for floating-point types (float and double)
    void addFloat(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec2Float(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec3Float(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec4Float(GLuint index, std::size_t offset) {
        glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addDouble(GLuint index, std::size_t offset) {
        glVertexAttribLPointer(index, 1, GL_DOUBLE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec2Double(GLuint index, std::size_t offset) {
        glVertexAttribLPointer(index, 2, GL_DOUBLE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec3Double(GLuint index, std::size_t offset) {
        glVertexAttribLPointer(index, 3, GL_DOUBLE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void addVec4Double(GLuint index, std::size_t offset) {
        glVertexAttribLPointer(index, 4, GL_DOUBLE, sizeof(vertex_t), (GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

    void printStatus() const {
        std::cout << "vao: " << VAO.getID() << '\n';
        std::cout << "vbo: " << VBO.getID() << '\n';
        std::cout << "ebo: " << EBO.getID() << '\n';
    }

    bool printBoundInfo() const {
        GLuint vao;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&vao);
        GLuint vbo;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&vbo);
        GLuint ebo;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&ebo);
        std::cout << "vao: " << (VAO.getID() == vao) << '\n';
        std::cout << "vbo: " << (VBO.getID() == vbo) << '\n';
        std::cout << "ebo: " << (EBO.getID() == ebo) << '\n';
        return (VAO.getID() == vao) && (VBO.getID() == vbo) && (EBO.getID() == ebo);
    }

    void draw() {
        #ifdef _DEBUG
        if (!DrawLogged) {
            std::cout << "7. Drawing Mesh...\n";
            DrawLogged = true;
        }
        #endif
        glDrawElements(PrimitiveType, static_cast<GLsizei>(EBO.index_count), GL_UNSIGNED_INT, 0);
    }

private:
    virtual void configureAttributes() {
        // Placeholder for custom attribute configuration logic
    }
};
