#pragma once
#include "Mesh.h"
#include "shader.h"
#include "texture.h"







struct vtx_2d_tex
{
	glm::vec2 position;
	glm::vec2 UVs;
    // Constructor to initialize position and UVs
    vtx_2d_tex(const glm::vec2& pos, const glm::vec2& uvs)
        : position(pos), UVs(uvs) {}
};

struct vtx_3d_tex
{
    glm::vec3 position;
    glm::vec2 UVs;
    // Constructor to initialize position and UVs
    vtx_3d_tex(const glm::vec3& pos, const glm::vec2& uvs): position(pos), UVs(uvs) {}
};


class tex_static_mesh_3D
{
public:
    Mesh<vtx_3d_tex, GL_STATIC_DRAW, GL_TRIANGLES> mesh;

    tex_static_mesh_3D()
    {

    };
    ~tex_static_mesh_3D()
    {

    };
    void init()
    {
        shader.init();
        texture.loadFromFile("slippery_bastard.jpg");



        
        GLfloat cube_vertices[] = {
            // Positions          // UVs
            // Front face
            -1.0f, -1.0f, -1.0f,  0.0f, 10.0f, // Bottom-left 0 0
             1.0f, -1.0f, -1.0f,  10.0f, 10.0f, // Bottom-right 1 0
             1.0f,  1.0f, -1.0f,  10.0f, 0.0f, // Top-right 1 1
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left 0 1

            // Back face
            -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, // Bottom-left
             1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right
            -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, // Top-left

            // Left face
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
            -1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
            -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

            // Right face
             1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
             1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

             // Top face
             -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
              1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
              1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
             -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

             // Bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
              1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
              1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Top-right
             -1.0f, -1.0f,  1.0f,  0.0f, 1.0f  // Top-left
        };

        GLuint cube_indices[] = {
            // Front face
            0, 2, 1,
            0, 3, 2,


            // Back face
            4, 5, 6,
            4, 6, 7,


            // Left face
            10,8, 9,
            10, 11, 8,

            // Right face
            12, 14, 13,
            12, 15, 14,

            // Top face
            16, 18, 17,
            16, 19, 18,

            // Bottom face
            22, 20, 21,
            23, 20, 22



        };
        

        mesh.init((vtx_3d_tex*)cube_vertices, sizeof(cube_vertices) / sizeof(vtx_3d_tex), cube_indices, sizeof(cube_indices) / sizeof(GLuint)); // Initialize Mesh2D

    }
    void init(vtx_3d_tex* vertices, size_t num_vertices, GLuint* indices, size_t num_indices, const char* vtx_shader_path, const char* frag_shader_path, const char* texture_path)
    {
        shader.initFromFiles(vtx_shader_path, frag_shader_path);
        texture.loadFromFile(texture_path);
        mesh.init(vertices, num_vertices, indices, num_indices); // //Initialize Mesh2D
    }
    void prepare_for_draw()
    {
        shader.use();
        texture.bind();
        mesh.bind();
    }
    void draw()
    {
        mesh.draw();
    }


private:
    Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST> texture;
    Shader shader;

};



class tex_dynamic_mesh_3D
{
public:
    glm::mat4 model_transform;

    tex_dynamic_mesh_3D(): model_transform(1.0f)
    {
        
    };
    ~tex_dynamic_mesh_3D()
    {

    };
    void init()
    {
        shader.init();
        texture.loadFromFile("resources/images/default_texture.bmp");




        GLfloat cube_vertices[] = {
            // Positions          // UVs
            // Front face
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left 0 0
             1.0f, -1.0f, -1.0f,  1.0f, 1.0f, // Bottom-right 1 0
             1.0f,  1.0f, -1.0f,  1.0f, 0.0f, // Top-right 1 1
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left 0 1

            // Back face
            -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, // Bottom-left
             1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right
            -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, // Top-left

            // Left face
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
            -1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
            -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

            // Right face
             1.0f, -1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
             1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

             // Top face
             -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, // Bottom-left (flipped UVs)
              1.0f,  1.0f, -1.0f,  1.0f, 1.0f, // Bottom-right (flipped UVs)
              1.0f,  1.0f,  1.0f,  1.0f, 0.0f, // Top-right (flipped UVs)
             -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, // Top-left (flipped UVs)

             // Bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
              1.0f, -1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
              1.0f, -1.0f,  1.0f,  1.0f, 1.0f, // Top-right
             -1.0f, -1.0f,  1.0f,  0.0f, 1.0f  // Top-left
        };

        GLuint cube_indices[] = {
            // Front face
            0, 2, 1,
            0, 3, 2,


            // Back face
            4, 5, 6,
            4, 6, 7,


            // Left face
            10,8, 9,
            10, 11, 8,

            // Right face
            12, 14, 13,
            12, 15, 14,

            // Top face
            16, 18, 17,
            16, 19, 18,

            // Bottom face
            22, 20, 21,
            23, 20, 22



        };


        //init((vtx_3d_tex*)cube_vertices, sizeof(cube_vertices) / sizeof(vtx_3d_tex), cube_indices, sizeof(cube_indices) / sizeof(GLuint)); // Initialize Mesh2D

    }
    void init(vtx_3d_tex* vertices, size_t num_vertices, GLuint* indices, size_t num_indices, const char* vtx_shader_path, const char* frag_shader_path, const char* texture_path)
    {
        shader.initFromFiles(vtx_shader_path, frag_shader_path);
        texture.loadFromFile(texture_path);
        mesh.init(vertices, num_vertices, indices, num_indices); // //Initialize Mesh2D
        mesh.addVec3(0, offsetof(vtx_3d_tex, position));
        mesh.addVec2(1, offsetof(vtx_3d_tex, UVs));
    }
    void prepare_for_draw()
    {
        //mesh.VAO.bind();
        //mesh.VBO.bind();
        //mesh.EBO.bind();

        shader.use();
        shader.setUniform("model", model_transform);

        texture.bind();

        mesh.bind();



    }
    void set_transform(glm::mat4 new_transform)
    {
        model_transform = new_transform;
        shader.setUniform("model", model_transform);

    }
    void draw()
    {
        mesh.draw();
    }


//private:
    Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST> texture;
    Shader shader;
    Mesh<vtx_3d_tex, GL_STATIC_DRAW, GL_TRIANGLES> mesh;

};


