#pragma once
#include "windowContent.h"
#include <windows.h>
#include <algorithm>
#include "Console.h"
#include "ent.h"
#include <SDL.h>

IMPORT_GLOBALS

void FocusConsoleWindow() {
    
}
void FocusSDLWindow(SDL_Window* sdlWindow) {
    // Bring SDL window to the foreground
    SDL_RaiseWindow(sdlWindow);
}

class level_editor_app : public Application<windowContent> {
private:

public:
    SDL_Event events;
    std::string fps_counter;
    float timeAccumulator = 0.0f;
    basic_console console;
    double previous_time = 0;
    double currentTime = 0;
    double elapsed_time = 0.0;
    level_editor_app(int argc, char** argv) : Application<windowContent>(), console(custom_state) {}

    void gui_tick(ImGuiContext& gui) override
    {
        
        currentTime = custom_state.t;
        elapsed_time = currentTime - previous_time;
        float& dt = globals.dt;
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

        if (custom_state.input_to_imgui)
        {
            console.Draw("Console");

        }
    }
    
    void gl_tick() override {

        auto& camera = custom_state.camController->camera;
        //std::cout << "position: ";
        //print_vec3(camera.position, true);
        //std::cout << "rotation: ";

        //print_vec2(camera.rotation, false);
        // camera.rotation = glm::vec2(globals.t, 0.0);

        //camera.rotation = glm::vec2(globals.t,0.0);

        //camera.rotate(glm::vec2(0.01, 0));
        auto& controller = custom_state.camController;

        // Check for events
        custom_state.last_cam_pos = custom_state.camController->camera.get_position();




        if (!custom_state.input_to_imgui)
        {
            custom_state.camController->CheckContinuousInput();
        }
        while (SDL_PollEvent(&events))
        {
            if (events.type == SDL_KEYDOWN && events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                // The `~` key was just pressed
                custom_state.input_to_imgui = !custom_state.input_to_imgui;
                // Toggle input_to_imgui or handle it as needed
            }
            // First, pass the event to the Controller
            if (!custom_state.input_to_imgui)
            {

                custom_state.camController->inputEvent(events);

            }
            // Check the boolean to decide where to pass the event
            else
            {
                ImGui_ImplSDL2_ProcessEvent(&events);
            }
        }
        custom_state.delta_cam_pos = custom_state.last_cam_pos - camera.get_position();


        // Update the Uniform Buffer of the camera
        custom_state.camController->camera.update_UBO();

        custom_state.brushes[0].draw();
        //console.ExecuteCommand("exec test_config.cfg");
        //brush::shader.setUniform("UV_offset_position", glm::vec3(0, custom_state.t, 0));

        //if (custom_state.camController->marked_for_processing)
       // {
        //    std::cout << ">";
        //    std::getline(std::cin, custom_state.camController->commands);
        //    parse_command(custom_state.camController->commands);
            //custom_state.camController->marked_for_processing = false;
       // }





        custom_state.t += (getDeltaTime());
        // set t
        custom_state.camController->camera.getUBO().updateData(sizeof(glm::mat4) * 2, sizeof(float), (const void*)&(custom_state.t));

        double d_dt = getDeltaTime();
        float dt = (float)d_dt;
        // set dt
        custom_state.camController->camera.getUBO().updateData((sizeof(glm::mat4) * 2) + sizeof(float), sizeof(float), (const void*)&dt);

        //camera.look_at(glm::vec3( 0.0,0.0,0.0));
        globals.t += globals.dt;

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



        console.target_application = &(custom_state);
        console.open = &(custom_state.input_to_imgui);



        // Initialized the camera / controller
        //custom_state.camController
        custom_state.camController = new level_editor_controller();
        custom_state.camController->camera.getUBO().init();

        // Bind it to slot 0
        custom_state.camController->camera.getUBO().bindBase(0);
        custom_state.camController->camera.set_position({ 10, 0, 0 });
        custom_state.camController->camera.look_at(glm::vec3(0, 0, 0));


        // TODO: Make add_brush's first arguments be relative to player when using ~, and second arguments can be exact position, direct dimensions with accent

        //custom_state.brushes.push_back({{ -5, 0, -5 }, { 10, 0.1, 10}});
        //console.ExecuteCommand("add_brush -5 0 -5 ~10 ~0.1 ~10");
        //brush B(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        custom_state.brushes.emplace_back();
        brush_setup();
        custom_state.brushes[0].prepare_for_draw();

        //custom_state.brushes[0].draw();
        //custom_state.brushes[0].extrude_triangle(a, 0, 1, 2, 0.5);
        //console.ExecuteCommand("exec test_config.cfg");

        //auto& a = custom_state.brushes[0].get_vertices();

        //a[5].material_ID = 1;
        //custom_state.brushes[0].update_vertices(a);
        //add_brush({ -5,0,-5 }, { 10,0.1 ,10 });
        //add_brush({ 0,0,1 }, { 1,1,1 });

        //custom_state.B.init({ 0,0,0 }, { 1,1,1 });
        // Disable the cursor
        


    }


    void draw_brushes()
    {
        size_t initial_size = custom_state.brushes.size();
        auto& brushes = custom_state.brushes;
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
