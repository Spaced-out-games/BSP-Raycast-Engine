#pragma once
#include "windowContent.h"


class cube_demo : public Application<windowContent> {
public:
    cube_demo(int argc, char** argv) : Application<windowContent>(argc, argv) {}

    void tick() override {


        auto& camera = getCustomState().camController.getPawn();
        auto& controller = getCustomState().camController;

        // Check for events
        getCustomState().last_cam_pos = getCustomState().camController.getPawn().getPosition();

        controller.inputEvent();
        controller.CheckContinuousInput();
        getCustomState().delta_cam_pos = getCustomState().last_cam_pos - camera.getPosition();
        glm::vec3 intersection = { 0,0,0 };






        getCustomState().cube.model_transform = glm::translate(glm::mat4(1.0f), intersection);
        getCustomState().cube.prepare_for_draw();
        getCustomState().cube.draw();



        // Update the Uniform Buffer of the camera
        getCustomState().camController.getPawn().updateUBO();



        getCustomState().brush_test.prepare_for_draw();
        getCustomState().brush_test.draw();


        float* a = (float*)getCustomState().brush_test.get_vertices();
        *a = getCustomState().t;

        getCustomState().brush_test.update_vertices((glm::vec3*)a);

        getCustomState().t += (*getDeltaTime());
    }

    void bootstrap() override {




        // Initialize the plane mesh
        vtx_3d_tex plane_points[] = {
            // Adjust the vertices so the plane faces the origin

            {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
            {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
            {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // Top-right
            {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},   // Top-left


        };
        GLuint plane_indices[] = {
            0, 2, 1,
            0, 3, 2,
            //0, 1, 2,
            //0, 2, 3,
        };
        getCustomState().plane.init(plane_points, sizeof(plane_points) / sizeof(vtx_3d_tex), plane_indices, sizeof(plane_indices) / sizeof(GLuint), "Bricks_001_vtx.glsl", "Bricks_001_frag.glsl", "Bricks_001.jpg");

        /*
        * If you are dealing with programmer block, here are your next steps:
        TODO:
        1. Define a triangle
        2. Create Bricks_001_frag.glsl (fragment shader for bricks. Basically, just color and parallax occlusion mapping, coordinates are derived from world space
        2. Create a way to create a BSP from a list of triangles
        3. Create a texture buffer through openGL to store the BSP tree
        4. Port the raycasting code to a fragment shader. Link it to the level BSP.
        5. Add more complicated shader support (Lighting effects, parallax, shader includes)
        6. Integrate a hash map for game objects that are stored per the node they belong to. Store this in a buffer and use CUDA to update the hash map.
        7. Traverse every node and compare collisions only to objects within that node (and of course, the level BSP from that leaf down)

        */


        // Initialize the cube mesh
        vtx_3d_tex cube_vertices[] = {
            // Front face
            {{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-1.0f,  1.0f, -1.0f}, {0.5f, 0.0f}}, // Top-left

            // Back face
            {{-1.0f, -1.0f,  1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-1.0f,  1.0f,  1.0f}, {0.5f, 0.0f}}, // Top-left

            // Left face
            {{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-1.0f,  1.0f, -1.0f}, {0.5f, 0.0f}}, // Top-left

            // Right face
            {{ 1.0f, -1.0f, -1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}}, // Top-right
            {{ 1.0f,  1.0f, -1.0f}, {0.5f, 0.0f}}, // Top-left

            // Top face
            {{-1.0f,  1.0f, -1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-1.0f,  1.0f,  1.0f}, {0.5f, 0.0f}}, // Top-left

            // Bottom face
            {{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f}}, // Bottom-left
            {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.5f}}, // Bottom-right
            {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-1.0f, -1.0f,  1.0f}, {0.5f, 0.0f}}  // Top-left
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
        getCustomState().cube.init(cube_vertices, sizeof(cube_vertices) / sizeof(vtx_3d_tex), cube_indices, sizeof(cube_indices) / sizeof(GLuint), "tex3d_shader_vtx_transformed.glsl", "tex3d_shader_frag.glsl", "Bricks_001.jpg");





        // Initialized the camera / controller
        getCustomState().camController.init(getWindowDimensions(), getDeltaTime());
        getCustomState().camController.getPawn().getUBO().init();

        // Bind it to slot 0
        getCustomState().camController.getPawn().getUBO().bindBase(0);
        getCustomState().camController.getPawn().setPosition({ 0, 1, 0 });
        getCustomState().brush_test.init();
        //getCustomState().camController.getPawn().lookAt({ 0, 0, 0.1 });
        // Disable the cursor
        SDL_ShowCursor(SDL_DISABLE);


    }
};