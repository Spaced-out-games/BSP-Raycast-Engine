#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>
#include <set>
#include <vector>


typedef struct brush_triangle
{
    glm::vec3 points[3];
    GLuint material_ID;

    glm::vec3 get_normal() {
        // Calculate the normal of the triangle
        glm::vec3 v1 = points[1] - points[0];
        glm::vec3 v2 = points[2] - points[0];
        return glm::normalize(glm::cross(v1, v2));
    }
};

typedef struct brush_face
{
    glm::vec3 normal;
    GLuint material_ID;

    // Constructor that takes triangle vertices and material ID
    brush_face(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, GLuint matID)
        : material_ID(matID)
    {
        // Calculate the normal using cross product
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        normal = glm::normalize(glm::cross(edge1, edge2)); // Normalize the normal
    }
};

typedef struct brush_triangle_reference
{
    GLuint indices[3];
};

typedef struct Brush
{
    static Shader shader;
    static Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> texture;
    static int r_wireframe;
    GLuint VAO;
    GLuint vertex_VBO;
    GLuint face_SSBO; // stores a brush_face[]
    GLuint solid_EBO;
    GLuint wireframe_EBO;

    // Set of unique points
    std::set<glm::vec3> points;
    // List of triangles
    std::vector<brush_triangle_reference> triangles;

    // List of per-face attributes
    std::vector<brush_face> face_data;
    size_t size;
    void destroy() {};
    Brush(glm::vec3 a, glm::vec3 b) { Brush(); }
    Brush() {
        // Define the unit cube vertices
        glm::vec3 point_cloud[] = {
            glm::vec3(0.0f, 0.0f, 0.0f), // 0
            glm::vec3(1.0f, 0.0f, 0.0f), // 1
            glm::vec3(1.0f, 1.0f, 0.0f), // 2
            glm::vec3(0.0f, 1.0f, 0.0f), // 3
            glm::vec3(0.0f, 0.0f, 1.0f), // 4
            glm::vec3(1.0f, 0.0f, 1.0f), // 5
            glm::vec3(1.0f, 1.0f, 1.0f), // 6
            glm::vec3(0.0f, 1.0f, 1.0f)  // 7
        };

        // Define the indices for the cube's faces (two triangles per face)
        GLuint indices[] = {
            // Bottom
            0, 1, 2, 0, 2, 3,
            // Top
            4, 5, 6, 4, 6, 7,
            // Front
            0, 1, 5, 0, 5, 4,
            // Back
            2, 3, 7, 2, 7, 6,
            // Left
            0, 3, 7, 0, 7, 4,
            // Right
            1, 2, 6, 1, 6, 5
        };

        // Populate the triangles vector
        for (size_t i = 0; i < sizeof(indices) / sizeof(GLuint); i += 3) {
            brush_triangle_reference triRef;
            triRef.indices[0] = indices[i];
            triRef.indices[1] = indices[i + 1];
            triRef.indices[2] = indices[i + 2];
            triangles.push_back(triRef);
        }

        // Define the face data for each triangle (normal and material ID)
        brush_face facedata[] = {
            brush_face(point_cloud[0], point_cloud[1], point_cloud[2], 0), // Bottom face
            brush_face(point_cloud[0], point_cloud[2], point_cloud[3], 0),
            brush_face(point_cloud[4], point_cloud[5], point_cloud[6], 0), // Top face
            brush_face(point_cloud[4], point_cloud[6], point_cloud[7], 0),
            brush_face(point_cloud[0], point_cloud[1], point_cloud[5], 0), // Front face
            brush_face(point_cloud[0], point_cloud[5], point_cloud[4], 0),
            brush_face(point_cloud[2], point_cloud[3], point_cloud[7], 0), // Back face
            brush_face(point_cloud[2], point_cloud[7], point_cloud[6], 0),
            brush_face(point_cloud[0], point_cloud[3], point_cloud[7], 0), // Left face
            brush_face(point_cloud[0], point_cloud[7], point_cloud[4], 0),
            brush_face(point_cloud[1], point_cloud[2], point_cloud[6], 0), // Right face
            brush_face(point_cloud[1], point_cloud[6], point_cloud[5], 0)
        };

        // Call init with the constructed data
        init(point_cloud, 8, indices, 36, facedata, 12);
    }

    void init()

    {
    // Define the unit cube vertices
    glm::vec3 point_cloud[] = {
        glm::vec3(0.0f, 0.0f, 0.0f), // 0
        glm::vec3(1.1f, 0.0f, 0.0f), // 1
        glm::vec3(1.0f, 1.0f, 0.0f), // 2
        glm::vec3(0.0f, 1.0f, 0.0f), // 3
        glm::vec3(0.0f, 0.0f, 1.0f), // 4
        glm::vec3(1.0f, 0.0f, 1.0f), // 5
        glm::vec3(1.0f, 1.0f, 1.0f), // 6
        glm::vec3(0.0f, 1.0f, 1.0f)  // 7
    };

    // Define the indices for the cube's faces (two triangles per face)
    GLuint indices[] = {
        // Bottom
        0, 1, 2, 0, 2, 3,
        // Top
        4, 5, 6, 4, 6, 7,
        // Front
        0, 1, 5, 0, 5, 4,
        // Back
        2, 3, 7, 2, 7, 6,
        // Left
        0, 3, 7, 0, 7, 4,
        // Right
        1, 2, 6, 1, 6, 5
    };

    // Populate the triangles vector
    for (size_t i = 0; i < sizeof(indices) / sizeof(GLuint); i += 3) {
        brush_triangle_reference triRef;
        triRef.indices[0] = indices[i];
        triRef.indices[1] = indices[i + 1];
        triRef.indices[2] = indices[i + 2];
        triangles.push_back(triRef);
    }

    // Define the face data for each triangle (normal and material ID)
    brush_face facedata[] = {
        brush_face(point_cloud[0], point_cloud[1], point_cloud[2], 0), // Bottom face
        brush_face(point_cloud[0], point_cloud[2], point_cloud[3], 1),
        brush_face(point_cloud[4], point_cloud[5], point_cloud[6], 1), // Top face
        brush_face(point_cloud[4], point_cloud[6], point_cloud[7], 1),
        brush_face(point_cloud[0], point_cloud[1], point_cloud[5], 2), // Front face
        brush_face(point_cloud[0], point_cloud[5], point_cloud[4], 2),
        brush_face(point_cloud[2], point_cloud[3], point_cloud[7], 3), // Back face
        brush_face(point_cloud[2], point_cloud[7], point_cloud[6], 3),
        brush_face(point_cloud[0], point_cloud[3], point_cloud[7], 4), // Left face
        brush_face(point_cloud[0], point_cloud[7], point_cloud[4], 4),
        brush_face(point_cloud[1], point_cloud[2], point_cloud[6], 5), // Right face
        brush_face(point_cloud[1], point_cloud[6], point_cloud[5], 5)
    };

    // Call init with the constructed data
    init(point_cloud, 8, indices, 36, facedata, 12);

    }


    void init(glm::vec3* point_cloud, size_t num_points, GLuint* indices, size_t num_indices, brush_face* facedata, size_t num_faces) {
        // Generate and bind the Vertex Array Object (VAO)
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind the Vertex Buffer Object (VBO) for vertices
        glGenBuffers(1, &vertex_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(glm::vec3), point_cloud, GL_STATIC_DRAW);

        // Set vertex attribute pointers (assuming location 0 is for positions)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // Generate and bind the Element Buffer Object (EBO) for triangle indices
        glGenBuffers(1, &solid_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solid_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);

        // Generate and bind the Shader Storage Buffer Object (SSBO) for face data (normals and material IDs)
        glGenBuffers(1, &face_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, face_SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, num_faces * sizeof(brush_face), facedata, GL_STATIC_DRAW);
        // Bind the SSBO to a binding point (e.g., binding point 0)
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, face_SSBO);

        // Unbind buffers to avoid accidental modification
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void prepare_for_draw()
    {
        shader.use();
        texture.bind();
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solid_EBO);
    }
    void draw() {
        // Use the shader program
        shader.use();

        // Bind the Vertex Array Object (VAO)
        glBindVertexArray(VAO);

        // Bind the texture (if you're using textures)
        texture.bind();
        int w = (int)r_wireframe;
        // Draw the solid faces using the Element Buffer Object (EBO)
        //shader.setUniform("glPrimitiveType", w);
        if (r_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset to filled polygon
        }
        else
        {
            
            glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
        }
        //

        // Optionally, if you want to draw wireframe as well
        ///*

        //*/

        // Unbind the VAO and texture
        glBindVertexArray(0);
        texture.unbind(); // Assuming you have a method to unbind the texture
    }

    // todo: make it actually set the right uniforms for the vertex shader (so that the fragment shader outputs pure green when in wireframe)
    static void set_wireframe_mode(int mode)
    {
        shader.setUniform("glPrimitiveType", mode);
        r_wireframe = mode;
    }
};





Shader Brush::shader;
Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> Brush::texture;
int Brush::r_wireframe = false;

void brush_setup()
{
    Brush::shader.initFromFiles("resources/shaders/brush_vertex.glsl", "resources/shaders/brush_frag.glsl");
    //Brush::shader.use();
    //GLfloat maxAniso;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    Brush::texture.loadFromFile("resources/images/atlas.png");
    Brush::set_wireframe_mode(0);

}

#include "entity.h"

typedef struct ent_brush : public ent
{
    Brush brush;
    enum : uint16_t {
        SET_WIREFRAME = 4,
        GET_WIREFRAME = 5
    };


    void init() override
    {
        ent::init();
        //Brush::init();
    }
    void draw() override
    {
        brush.prepare_for_draw();
        brush.draw();

    }
    void tick() override
    {
        draw(); // simply draw it.
    }
    void exec(ent_request stream) override{
        bool b = 0;
        switch (stream.command_ID) {
        case NULL_OPERATION:
            stream.command_ID = 0xffff; // Command executed successfully
            break; // No-op packet
        case INIT:
            init();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case TICK:
            tick();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case DRAW:
            draw();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case SET_WIREFRAME:
            b = 0;// stream.read_next<bool>(0);
            Brush::set_wireframe_mode(b);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case GET_WIREFRAME:
            //stream.write_next<bool>(static_cast<bool>r_wireframe, 0);
            stream.command_ID = 0xffff; // Command executed successfully

        default:
            stream.command_ID = 0xfffe; // Command executed successfully
            break;
        }
    }
};
