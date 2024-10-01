#include <glm/glm.hpp>
//#include "Mesh.h"
#include "shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>

typedef struct nBrush_triangle
{
    glm::vec3 points[3];
    glm::vec3 normal;  // Default initialization will be done in the constructor.
    uint16_t material_ID;

    // Constructor to initialize normal and material_ID
    nBrush_triangle() : normal(0.0f, 0.0f, 0.0f), material_ID(0) {}

    void move(const glm::vec3& offset)
    {
        points[0] += offset;
        points[1] += offset;
        points[2] += offset;
    }
} nBrush_triangle;


















class nBrush {
public:
    static Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> texture;
    static Shader shader;
    static bool use_wireframe; // Static boolean to toggle wireframe
    // Constructor, Destructor, etc. (defined as before)
    nBrush() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_positions);
        glGenBuffers(1, &VBO_materials);
        createCube();
    }
    nBrush(const std::vector<nBrush_triangle>& triangles) {};
    ~nBrush() {
        disable_edit();  // Ensure that editing is disabled and the buffer is unmapped before deletion.
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO_positions != 0) {
            glDeleteBuffers(1, &VBO_positions);
        }
        if (VBO_materials != 0) {
            glDeleteBuffers(1, &VBO_materials);
        }
    }

    // Move Constructor
    nBrush(nBrush&& other) noexcept
        : VAO(other.VAO),
        VBO_positions(other.VBO_positions),
        VBO_materials(other.VBO_materials),
        triangles(std::move(other.triangles)) {

        // Invalidate the OpenGL state of the source object
        other.VAO = 0;
        other.VBO_positions = 0;
        other.VBO_materials = 0;
    }

    // Move Assignment Operator
    nBrush& operator=(nBrush&& other) noexcept {
        if (this != &other) {
            // Clean up existing resources
            if (VAO != 0) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO_positions != 0) {
                glDeleteBuffers(1, &VBO_positions);
            }
            if (VBO_materials != 0) {
                glDeleteBuffers(1, &VBO_materials);
            }

            // Transfer resources from 'other' to 'this'
            VAO = other.VAO;
            VBO_positions = other.VBO_positions;
            VBO_materials = other.VBO_materials;
            triangles = std::move(other.triangles);

            // Invalidate the OpenGL state of the source object
            other.VAO = 0;
            other.VBO_positions = 0;
            other.VBO_materials = 0;
        }
        return *this;
    }

    // Delete copy constructor and copy assignment operator to avoid accidental copies
    nBrush(const nBrush&) = delete;
    nBrush& operator=(const nBrush&) = delete;

    void bind() const {
        glBindVertexArray(VAO);
    }
    void unbind() const {
        glBindVertexArray(0);
    }
    void draw() const {
        bind();
        glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3); // Draw the triangles
        unbind();
    }
    // Opens the VBO for editing and returns a pointer
    float* enable_edit() {
        if (!isEditable) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // Map the buffer to client memory (GL_WRITE_ONLY since you're editing)
            mappedBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            if (mappedBuffer) {
                isEditable = true;
                return static_cast<float*>(mappedBuffer); // Return pointer to the mapped buffer
            }
            else {
                std::cerr << "Failed to map VBO for editing!" << std::endl;
                return nullptr;
            }
        }
        return nullptr;
    }

    // Closes the VBO after editing
    void disable_edit() {
        if (isEditable) {
            // Unmap the buffer after editing
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            if (mappedBuffer) {
                glUnmapBuffer(GL_ARRAY_BUFFER);
                mappedBuffer = nullptr;
            }
            isEditable = false;
        }
    }

    // Clears the triangle data and updates the VBO
    void clear() {
        triangles.clear(); // Clear the vector of triangles
        //setupVBO();        // Don't update the VBO after clearing, as this means the nBrush is no longer editable
    }
    void set_mat(size_t triangle_index, uint16_t mat_ID) {
        if (triangle_index >= triangles.size()) {
            std::cerr << "Invalid triangle index!" << std::endl;
            return;
        }

        // Update the material ID of the triangle
        triangles[triangle_index].material_ID = mat_ID;

        // Update the VBO data to reflect this change
        glBindBuffer(GL_ARRAY_BUFFER, VBO_materials);

        // Calculate the offset to the material ID in the VBO and update it
        glBufferSubData(GL_ARRAY_BUFFER, triangle_index * sizeof(uint16_t), sizeof(uint16_t), &mat_ID);

        // Unbind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    private:
        GLuint VAO;
        GLuint VBO; // Unified VBO
        GLuint materialVBO;
        std::vector<nBrush_triangle> triangles; // Store triangles in a vector. MAKE A METHOD TO GET A REFERENCE TO THIS
        bool isEditable; // Flag to track if the VBO is currently mapped for editing

        void setupVBO() {
            size_t numTriangles = triangles.size();
            size_t numVertices = numTriangles * 3; // Each triangle has 3 vertices

            // Calculate buffer size for vertex positions and material IDs
            size_t bufferSize = numVertices * sizeof(glm::vec3) + numTriangles * sizeof(uint16_t);

            // Create buffer data
            std::vector<float> bufferData(bufferSize / sizeof(float)); // Buffer for vertex positions
            std::vector<uint16_t> materialIDs(numTriangles); // Buffer for material IDs

            size_t vertexIndex = 0;

            for (size_t i = 0; i < numTriangles; ++i) {
                const nBrush_triangle& triangle = triangles[i];

                // Add triangle vertices to the buffer
                for (int j = 0; j < 3; ++j) {
                    bufferData[vertexIndex++] = triangle.points[j].x;
                    bufferData[vertexIndex++] = triangle.points[j].y;
                    bufferData[vertexIndex++] = triangle.points[j].z;
                }

                // Store the material ID
                materialIDs[i] = triangle.material_ID;
            }

            // Generate and bind VAO and VBO
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // Upload vertex positions
            glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

            // Setup vertex attribute for positions
            glEnableVertexAttribArray(0); // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Position data

            // Upload material IDs
            GLuint materialVBO;
            glGenBuffers(1, &materialVBO);
            glBindBuffer(GL_ARRAY_BUFFER, materialVBO);
            glBufferData(GL_ARRAY_BUFFER, materialIDs.size() * sizeof(uint16_t), materialIDs.data(), GL_STATIC_DRAW);

            // Setup vertex attribute for material ID
            glEnableVertexAttribArray(1);
            glVertexAttribIPointer(1, 1, GL_UNSIGNED_SHORT, sizeof(uint16_t), (void*)0); // Material ID data

            // Unbind the VAO
            glBindVertexArray(0);
        }
        void createCube() {
            // Define the vertices of the cube
            nBrush_triangle cubeTriangles[12];

            glm::vec3 vertices[8] = {
                { -1.0f, -1.0f, -1.0f }, // 0: Back bottom left
                {  1.0f, -1.0f, -1.0f }, // 1: Back bottom right
                {  1.0f,  1.0f, -1.0f }, // 2: Back top right
                { -1.0f,  1.0f, -1.0f }, // 3: Back top left
                { -1.0f, -1.0f,  1.0f }, // 4: Front bottom left
                {  1.0f, -1.0f,  1.0f }, // 5: Front bottom right
                {  1.0f,  1.0f,  1.0f }, // 6: Front top right
                { -1.0f,  1.0f,  1.0f }  // 7: Front top left
            };

            // Define the triangles for each face of the cube
            // Back face
            cubeTriangles[0] = { { vertices[0], vertices[1], vertices[2] }, {0.0f, 0.0f, -1.0f}, 0 };
            cubeTriangles[1] = { { vertices[0], vertices[2], vertices[3] }, {0.0f, 0.0f, -1.0f}, 0 };

            // Front face
            cubeTriangles[2] = { { vertices[4], vertices[5], vertices[6] }, {0.0f, 0.0f, 1.0f}, 1 };
            cubeTriangles[3] = { { vertices[4], vertices[6], vertices[7] }, {0.0f, 0.0f, 1.0f}, 1 };

            // Left face
            cubeTriangles[4] = { { vertices[0], vertices[4], vertices[7] }, {-1.0f, 0.0f, 0.0f}, 2 };
            cubeTriangles[5] = { { vertices[0], vertices[7], vertices[3] }, {-1.0f, 0.0f, 0.0f}, 2 };

            // Right face
            cubeTriangles[6] = { { vertices[1], vertices[2], vertices[6] }, {1.0f, 0.0f, 0.0f}, 3 };
            cubeTriangles[7] = { { vertices[1], vertices[6], vertices[5] }, {1.0f, 0.0f, 0.0f}, 3 };

            // Top face
            cubeTriangles[8] = { { vertices[3], vertices[2], vertices[6] }, {0.0f, 1.0f, 0.0f}, 4 };
            cubeTriangles[9] = { { vertices[3], vertices[6], vertices[7] }, {0.0f, 1.0f, 0.0f}, 4 };

            // Bottom face
            cubeTriangles[10] = { { vertices[0], vertices[1], vertices[5] }, {0.0f, -1.0f, 0.0f}, 5 };
            cubeTriangles[11] = { { vertices[0], vertices[5], vertices[4] }, {0.0f, -1.0f, 0.0f}, 5 };

            // Now set up the VBO with these cube triangles
            setupVBO(cubeTriangles, 12);
        }

        void* mappedBuffer; // Pointer to the mapped buffer for editing
};