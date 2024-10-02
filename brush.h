#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>


// to-do: complete re-write lmao

typedef struct Brush_vertex_t {
    glm::vec3 position;    // Vertex position
    glm::vec3 normal;      // Vertex normal
    uint8_t material_ID;   // Material ID for triplanar mapping
} Brush_vertex_t;
typedef struct Brush {
    static Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> texture;
    glm::vec3 UV_offset_position = { 0,0,0 };
    static Shader shader;
    static bool use_wireframe; // Static boolean to toggle wireframe
    float t = 0.0;
    glm::vec3 get_origin()
    {
        return get_vertices()[0].position;
    }
    Brush() {};
    Brush(glm::vec3 position, glm::vec3 dimensions) {
        init(position, dimensions);
    }

    Mesh<Brush_vertex_t, GL_STATIC_DRAW, GL_TRIANGLES> mesh;        // Solid mesh
    Mesh<Brush_vertex_t, GL_STATIC_DRAW, GL_LINES> wireframe_mesh;  // Wireframe mesh

    Brush_vertex_t* buffer_ptr = nullptr;

    void destroy() {
        mesh.destroy();
        wireframe_mesh.destroy();
    }

    void init(glm::vec3 position1, glm::vec3 position2) {
        // Calculate dimensions based on the two positions
        glm::vec3 dimensions = position2 - position1;

        // Define the cuboid vertices using the two positions
        Brush_vertex_t cuboid_vertices[] = {
            // Front face (material ID 0, normal facing forward)
            { { position1.x, position1.y, position1.z }, {-1.0f, 0.0f, 0.0f}, 6 },  // 0: Bottom-left-front
            { { position2.x, position1.y, position1.z }, { 1.0f, 0.0f, 0.0f }, 6 },  // 1: Bottom-right-front
            { { position2.x, position2.y, position1.z }, {0.0f, 0.0f, -1.0f}, 0 },  // 2: Top-right-front
            { { position1.x, position2.y, position1.z }, { 0.0f, 1.0f, 0.0f }, 0 },  // 3: Top-left-front

            // Back face (material ID 1, normal facing backward)
            { { position1.x, position1.y, position2.z }, { 0.0f, 0.0f, 1.0f },0},  // 4: Bottom-left-back
            { { position2.x, position1.y, position2.z }, { 0.0f, -1.0f, 0.0f },  0 },  // 5: Bottom-right-back
            { { position2.x, position2.y, position2.z }, { 0.0f, 0.0f, 0.0f }, 0 },  // 6: Top-right-back
            { { position1.x, position2.y, position2.z }, { 0.0f, 0.0f, 0.0f }, 0 }   // 7: Top-left-back
        };
        /*
        GLuint cuboid_indices[] = {
            0, 1, 2, 2, 3, 0, // Front
            4, 7, 6, 6, 5, 4, // Back
            0, 3, 7, 7, 4, 0, // Left
            1, 5, 6, 6, 2, 1, // Right
            3, 2, 6, 6, 7, 3, // Top
            1, 0, 5, 5, 0, 4  // Bottom
        };
        */
        GLuint cuboid_indices[] = {
            // Front face (Triangles 0 and 1)
            //1, 2, 0,   // First triangle (bottom-left-front is the third index)
            //2, 3, 0,   // Second triangle
            0,1,2,
            3,0,2,

            // Back face (Triangles 2 and 3)
            6, 5, 4,   // First triangle (bottom-left-back is the third index)
            7, 6, 4,   // Second triangle

            // Left face (Triangles 4 and 5)
            7, 4, 0,   // First triangle (bottom-left-front is the third index)
            3, 7, 0,   // Second triangle

            // Right face (Triangles 6 and 7)
            5, 6, 1,   // First triangle (bottom-right-front is the third index)
            6, 2, 1,   // Second triangle

            // Top face (Triangles 8 and 9)
            2, 6, 3,   // First triangle (top-left-front is the third index)
            6, 7, 3,   // Second triangle

            // Bottom face (Triangles 10 and 11)
            1,0,5,   // First triangle (bottom-left-front is the third index)
            0,4,5   // Second triangle
        };

        GLuint wireframe_indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front
            4, 5, 5, 6, 6, 7, 7, 4, // Back
            0, 4, 1, 5, 2, 6, 3, 7  // Connecting edges
        };

        // Initialize the solid mesh
        mesh.init(cuboid_vertices, sizeof(cuboid_vertices) / sizeof(Brush_vertex_t), cuboid_indices, sizeof(cuboid_indices) / sizeof(GLuint));
        mesh.addVec3(0, offsetof(Brush_vertex_t, position));   // Add position attribute to shader
        mesh.addVec3(1, offsetof(Brush_vertex_t, normal));     // Add normal attribute to shader
        mesh.addUInt8(2, offsetof(Brush_vertex_t, material_ID)); // Add material ID attribute to shader

        // Initialize the wireframe mesh (using only positions)
        wireframe_mesh.init(cuboid_vertices, sizeof(cuboid_vertices) / sizeof(Brush_vertex_t), wireframe_indices, sizeof(wireframe_indices) / sizeof(GLuint));
        wireframe_mesh.addVec3(0, offsetof(Brush_vertex_t, position)); // Add position attribute for wireframe

        // Get a pointer to the array buffer for solid mesh
        buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        if (buffer_ptr) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else {
            std::cout << "Buffer map failed: Cannot read/write to the VBO\n";
            exit(1);
        }
    }


    void prepare_for_draw() {
        shader.use();
        //shader.setUniform("model", glm::mat4(1.0f));
        texture.bind();

        if (use_wireframe) {
            wireframe_mesh.bind();  // Use the wireframe mesh
        }
        else {
            mesh.bind();  // Use the solid mesh
        }
    }

    void draw() {

        if (use_wireframe) {
            wireframe_mesh.draw();  // Draw as wireframe
        }
        else {
            //std::vector<Brush_vertex_t> temp = get_vertices()

            mesh.draw();  // Draw as solid

           // t += 0.001;
        }
    }

    std::vector<Brush_vertex_t> get_vertices() {
        std::vector<Brush_vertex_t> vertices(8);  // Allocate space for 8 vertices

        // Bind the mesh to ensure the VBO is active
        mesh.bind();

        // Map the buffer to read the vertices
        Brush_vertex_t* buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        if (buffer_ptr) {
            // Copy the data into the std::vector
            std::copy(buffer_ptr, buffer_ptr + 8, vertices.begin());

            // Unmap the buffer after reading
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else {
            std::cout << "Buffer map failed: Cannot read from the VBO\n";
            exit(1);
        }

        return vertices;
    }


    void update_vertices(const std::vector<Brush_vertex_t>& new_vertices) {
        mesh.bind();  // Bind solid mesh

        // Define an array to store the calculated normals for each vertex
        std::vector<glm::vec3> vertex_normals = {};

        // Define the cuboid triangles (same as cuboid_indices)
        const GLuint cuboid_indices[] = {
            // Front face
            0, 1, 2, 3, 0, 2,
            // Back face
            6, 5, 4, 7, 6, 4,
            // Left face
            7, 4, 0, 3, 7, 0,
            // Right face
            5, 6, 1, 6, 2, 1,
            // Top face
            2, 6, 3, 6, 7, 3,
            // Bottom face
            1, 0, 5, 0, 4, 5
        };

        // Iterate over each triangle, calculate its normal, and accumulate it for each vertex
        for (size_t i = 0; i < sizeof(cuboid_indices) / sizeof(GLuint); i += 3) {
            // Get the vertices of the current triangle
            const Brush_vertex_t& v0 = new_vertices[cuboid_indices[i]];
            const Brush_vertex_t& v1 = new_vertices[cuboid_indices[i + 1]];
            const Brush_vertex_t& v2 = new_vertices[cuboid_indices[i + 2]];

            // Calculate two edges of the triangle
            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;

            // Calculate the triangle's normal using the cross product
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            // Accumulate this normal to each vertex of the triangle
            vertex_normals[cuboid_indices[i]] += normal;
            vertex_normals[cuboid_indices[i + 1]] += normal;
            vertex_normals[cuboid_indices[i + 2]] += normal;
        }

        // Normalize the accumulated normals to ensure they are unit vectors
        for (auto& normal : vertex_normals) {
            normal = glm::normalize(normal);
        }

        // Map the buffer to update the vertices
        buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (buffer_ptr) {
            for (size_t i = 0; i < 8; ++i) {
                buffer_ptr[i] = new_vertices[i];
                buffer_ptr[i].normal = vertex_normals[i]; // Set the calculated normal
            }

            // Unmap the buffer after updating
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else {
            std::cout << "Buffer map failed: Cannot write to the VBO\n";
            exit(1);
        }

        // Update wireframe mesh (without normals)
        wireframe_mesh.bind();
        buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (buffer_ptr) {
            for (size_t i = 0; i < 8; ++i) {
                buffer_ptr[i] = new_vertices[i];
            }

            // Unmap the buffer after updating
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else {
            std::cout << "Buffer map failed: Cannot write to the VBO\n";
            exit(1);
        }
    }

    
    void move_vertex(size_t index, const glm::vec3& new_position) {
        if (index >= 8) {
            std::cerr << "Vertex index out of bounds" << std::endl;
            return;
        }

        // Update the solid mesh vertex
        mesh.bind(); // Bind solid mesh
        buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        if (buffer_ptr) {
            buffer_ptr[index].position = new_position; // Update position
            // You may want to update normals or material IDs if needed
            glUnmapBuffer(GL_ARRAY_BUFFER); // Unmap after writing
        }
        else {
            std::cout << "Buffer map failed: Cannot write to the VBO\n";
            exit(1);
        }

        // Update the wireframe mesh vertex
        wireframe_mesh.bind(); // Bind wireframe mesh
        buffer_ptr = (Brush_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        if (buffer_ptr) {
            buffer_ptr[index].position = new_position; // Update position
            glUnmapBuffer(GL_ARRAY_BUFFER); // Unmap after writing
        }
        else {
            std::cout << "Buffer map failed: Cannot write to the VBO\n";
            exit(1);
        }
    }

    void extrude_triangle(const glm::vec3& direction, size_t index1, size_t index2, size_t index3, float distance) {
        // Get the current vertices
        std::vector<Brush_vertex_t> vertices = get_vertices();

        // Create new vertices for the extruded triangle
        Brush_vertex_t new_vertex1 = vertices[index1];
        Brush_vertex_t new_vertex2 = vertices[index2];
        Brush_vertex_t new_vertex3 = vertices[index3];

        // Move the vertices in the extrusion direction
        new_vertex1.position += glm::normalize(direction) * distance;
        new_vertex2.position += glm::normalize(direction) * distance;
        new_vertex3.position += glm::normalize(direction) * distance;

        // Add the new vertices to the vertex list
        vertices.push_back(new_vertex1);
        vertices.push_back(new_vertex2);
        vertices.push_back(new_vertex3);

        // Update indices to include the new triangle
        std::vector<GLuint> indices = {
            // Existing indices
            0, 1, 2, 3, 0, 2,
            6, 5, 4, 7, 6, 4,
            7, 4, 0, 3, 7, 0,
            5, 6, 1, 6, 2, 1,
            2, 6, 3, 6, 7, 3,
            1, 0, 5, 0, 4, 5,

            // New indices for the extruded triangle
            static_cast<GLuint>(vertices.size() - 3), static_cast<GLuint>(vertices.size() - 2), static_cast<GLuint>(vertices.size() - 1)
        };

        // Update vertex and index buffers
        update_vertices(vertices);
        mesh.EBO.update_indices(indices.data(), indices.size());
    }
} Brush;



// Define static members
Shader Brush::shader;
Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> Brush::texture;
bool Brush::use_wireframe = false;

void brush_setup()
{
    Brush::shader.initFromFiles("resources/shaders/brush_vertex.glsl", "resources/shaders/brush_frag.glsl");
    Brush::shader.use();
    //GLfloat maxAniso;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    Brush::texture.loadFromFile("resources/images/atlas.png");
    
}
