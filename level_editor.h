#pragma once
#include "windowContent.h"
#include <windows.h>
#include <algorithm>
#include "Console.h"


void FocusConsoleWindow() {
    
}
void FocusSDLWindow(SDL_Window* sdlWindow) {
    // Bring SDL window to the foreground
    SDL_RaiseWindow(sdlWindow);
}

class level_editor_app : public Application<windowContent> {
private:

public:
    std::string fps_counter;
    float timeAccumulator = 0.0f;
    basic_console console;
    double previous_time = 0;
    double currentTime = 0;
    double elapsed_time = 0.0;
    level_editor_app(int argc, char** argv) : Application<windowContent>(argc, argv), console(getCustomState()) {}

    void gui_tick(ImGuiContext& gui) override
    {
        currentTime = getCustomState().t;
        elapsed_time = currentTime - previous_time;
        double& dt = *getDeltaTime();
        if (elapsed_time >= 0.05)  // 0.05 seconds corresponds to 20 updates per second
        {
            double fps = 1.0 / dt;  // Calculate FPS if dt > 0
            fps_counter = "FPS: " + std::to_string((int)fps);  // Update the FPS counter
            previous_time = currentTime;  // Update the previous time
        }
        if (dt > 0.0f)
        {
            ImGui::GetBackgroundDrawList()->AddText(NULL, 50.0f, ImVec2(30, 30), IM_COL32(255, 255, 255, 255), fps_counter.c_str());
        }

        if (getCustomState().input_to_imgui)
        {
            console.Draw("Console");

        }
    }
    
    void gl_tick() override {

        auto& camera = getCustomState().camController->getPawn();
        auto& controller = getCustomState().camController;

        // Check for events
        getCustomState().last_cam_pos = getCustomState().camController->getPawn().getPosition();




        if (!getCustomState().input_to_imgui)
        {
            getCustomState().camController->CheckContinuousInput();
        }
        while (SDL_PollEvent(&events))
        {
            if (events.type == SDL_KEYDOWN && events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                // The `~` key was just pressed
                getCustomState().input_to_imgui = !getCustomState().input_to_imgui;
                // Toggle input_to_imgui or handle it as needed
            }
            // First, pass the event to the Controller
            if (!getCustomState().input_to_imgui)
            {

                getCustomState().camController->inputEvent(events);

            }
            // Check the boolean to decide where to pass the event
            else
            {
                ImGui_ImplSDL2_ProcessEvent(&events);
            }
        }
        getCustomState().delta_cam_pos = getCustomState().last_cam_pos - camera.getPosition();


        // Update the Uniform Buffer of the camera
        getCustomState().camController->getPawn().updateUBO();

        draw_brushes();
        //console.ExecuteCommand("exec test_config.cfg");
        //brush::shader.setUniform("UV_offset_position", glm::vec3(0, getCustomState().t, 0));

        //if (getCustomState().camController->marked_for_processing)
       // {
        //    std::cout << ">";
        //    std::getline(std::cin, getCustomState().camController->commands);
        //    parse_command(getCustomState().camController->commands);
            //getCustomState().camController->marked_for_processing = false;
       // }





        getCustomState().t += (*getDeltaTime());
    }
    


    void bootstrap() override {

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

        console.target_application = &(getCustomState());
        console.open = &(getCustomState().input_to_imgui);



        // Initialized the camera / controller
        //getCustomState().camController
        getCustomState().camController = new level_editor_controller();
        getCustomState().camController->init(getWindowDimensions(), getDeltaTime(), &(getCustomState()));
        getCustomState().camController->getPawn().getUBO().init();

        // Bind it to slot 0
        getCustomState().camController->getPawn().getUBO().bindBase(0);
        getCustomState().camController->getPawn().setPosition({ 0, 5, 0 });
        brush_setup();

        // TODO: Make add_brush's first arguments be relative to player when using ~, and second arguments can be exact position, direct dimensions with accent

        //getCustomState().brushes.push_back({{ -5, 0, -5 }, { 10, 0.1, 10}});
        console.ExecuteCommand("add_brush -5 0 -5 ~10 ~0.1 ~10");
        //brush B(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        getCustomState().brushes.emplace_back(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        //getCustomState().brushes[0].extrude_triangle(a, 0, 1, 2, 0.5);
        //console.ExecuteCommand("exec test_config.cfg");

        //auto& a = getCustomState().brushes[0].get_vertices();

        //a[5].material_ID = 1;
        //getCustomState().brushes[0].update_vertices(a);
        //add_brush({ -5,0,-5 }, { 10,0.1 ,10 });
        //add_brush({ 0,0,1 }, { 1,1,1 });

        //getCustomState().B.init({ 0,0,0 }, { 1,1,1 });
        // Disable the cursor
        


    }


    void draw_brushes()
    {
        size_t initial_size = getCustomState().brushes.size();
        auto& brushes = getCustomState().brushes;
        // Loop through existing brushes
        for (size_t i = 0; i < initial_size; ++i) {

            //brushes[i].prepare_for_draw();
            brushes[i].draw();

        }

    }
    
    //todo: 
        // exec <filename>
        // save <filename.brushlist>
        // load <filename.brushlist>
        // compile <filename.bsp>
        // optimize <start_seed> <end_seed>
        // sleep <seconds>
        // tp <position>
        // lookat <position>
        // noclip
        // tp_brush <position>
        // 
        // It might be better to structure commands into their own file.
        // 
        // 
        // help (command)
        //typedef struct console_command
        //{
            // The actual function
            // std::function function;
            // The name of the invoked function
            // std::string convar_name;
            // Description of the function
            // std::string description;
            // Defines the number of arguments there are for each overload
            // std::vector<std::vector<uint8_t>> argument_types;


        //};

};
// C:\Users\devin\source\repos\BSP-Raycast-Engine\test_config.cfg
