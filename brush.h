#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "res_shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>
#include <set>
#include <vector>
#include "ent.h"

void brush_setup();
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
    brush_face() = default;
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
    brush_triangle_reference() = default;
    GLuint indices[3];
    brush_face face_data;
};

class ent_brush : public ent  // Inherit from ent
{
public:
    static res_shader shader;
    static Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> texture;
    GLuint VAO = 0;
    GLuint vertex_VBO = 0;
    GLuint face_SSBO = 0; // stores a brush_face[]
    GLuint solid_EBO = 0;
    GLuint wireframe_EBO = 0;
    // needs implemented
    virtual void init() override {};
    virtual void tick() override {};

    virtual void exec(const ent_command& command) override {};

    const char* get_name() const override
    {
        return "ent_brush";
    }

    // Set of unique points
    std::set<glm::vec3> points;
    // List of triangles
    std::vector<brush_triangle_reference> triangles;

    size_t size;

    // Constructor with parameters
    ent_brush(const std::set<glm::vec3>& unique_points_set, const std::vector<brush_triangle_reference>& triangles_input)
        : triangles(triangles_input) // Initialize triangles directly
    {
        // set => glm::vec3[]
        size_t num_points = unique_points_set.size();

        // Allocate memory for the point cloud
        glm::vec3* point_cloud = new glm::vec3[num_points];

        // Fill the point cloud using iterators
        size_t i = 0;
        for (const auto& point : unique_points_set)
        {
            point_cloud[i++] = point;
        }

        // Get the index buffer size
        size_t num_triangles = triangles.size();

        // Allocate memory for the index buffer
        GLuint* indices = new GLuint[num_triangles * 3];

        // Copy the indices
        for (size_t i = 0; i < num_triangles; i++)
        {
            indices[i * 3] = triangles[i].indices[0];
            indices[i * 3 + 1] = triangles[i].indices[1];
            indices[i * 3 + 2] = triangles[i].indices[2];
        }

        // Allocate memory for the face data
        brush_face* facedata = new brush_face[num_triangles];
        // Copy the face data
        for (size_t i = 0; i < num_triangles; i++)
        {
            facedata[i] = triangles[i].face_data;
        }

        // Create the mesh
        make_mesh(point_cloud, num_points, indices, num_triangles * 3, facedata, num_triangles);

        // Free the buffers as they are now on the GPU
        delete[] indices;
        delete[] point_cloud;
        delete[] facedata;
    }

    ent_brush() : ent_brush({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }) {}

    // Creates a cube by default
    ent_brush(const glm::vec3& start, const glm::vec3& stop) {
        // Define the unit cube vertices based on start and stop positions
        glm::vec3 point_cloud[] = {
            glm::vec3(start.x, start.y, start.z), // 0
            glm::vec3(stop.x, start.y, start.z),  // 1
            glm::vec3(stop.x, stop.y, start.z),   // 2
            glm::vec3(start.x, stop.y, start.z),  // 3
            glm::vec3(start.x, start.y, stop.z),  // 4
            glm::vec3(stop.x, start.y, stop.z),   // 5
            glm::vec3(stop.x, stop.y, stop.z),    // 6
            glm::vec3(start.x, stop.y, stop.z)    // 7
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
        make_mesh(point_cloud, 8, indices, 36, facedata, 12);
    }

    // Destructor
    ~ent_brush() {
        // Cleanup OpenGL resources
        glDeleteBuffers(1, &vertex_VBO);
        glDeleteBuffers(1, &solid_EBO);
        glDeleteBuffers(1, &face_SSBO);
        glDeleteVertexArrays(1, &VAO);
    }
    // Delete copy constructor and copy assignment operator
    ent_brush(const ent_brush&) {}// = delete;

    
    // this is legal
    ent_brush& operator=(const ent_brush&)  = delete;

    // Optionally, you can also explicitly define move constructor and move assignment operator
    ent_brush(ent_brush&&) noexcept = default;
    ent_brush& operator=(ent_brush&&) noexcept = default;


    void prepare_for_draw()
    {
        shader.use();
        texture.bind();
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solid_EBO);
    }

    void draw() {
        //prepare_for_draw();
        // Use the shader program

        // Bind the Vertex Array Object (VAO)
        //glBindVertexArray(VAO);

        // Bind the texture (if you're using textures)

        //showOpenGLState();

        glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
    }

    //static void init() { brush_setup(); }
        


    private:
        // Makes a mesh from a collection of points. 
        void make_mesh(glm::vec3* point_cloud, size_t num_points, GLuint* indices, size_t num_indices, brush_face* facedata, size_t num_faces) {
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
};

res_shader ent_brush::shader;

// Texture and other resources should probably have their constructors deferred to a resource_manager that returns a pointer to the type, in which case, entities bind before the next draw call.
// This can build off of the planned ent_manager / server system that is on the horizon...
Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> ent_brush::texture;

// Due to the way wireframes work in OpenGL, this might as well be in Globals. Globals should have an update() function so that uniforms (like r_rendermode / r_wireframe) can be set each frame

void brush_setup()
{
    ent_brush::shader = std::move(res_shader("resources/shaders/brush_vertex.glsl", "resources/shaders/brush_frag.glsl"));
    //ent_brush::shader.use();
    //GLfloat maxAniso;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    ent_brush::texture.loadFromFile("resources/images/atlas.png");
    globals.set_render_mode(1);

}