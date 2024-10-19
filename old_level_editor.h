#pragma once
#include "windowContent.h"
#include <windows.h>
#include <algorithm>
#include "Console.h"
#include "entity.h"


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
    double previous_time = 0.0;
    double currentTime = 0.0;
    double elapsed_time = 0.0;
    level_editor_app(int argc, char** argv) : Application<windowContent>(argc, argv), console(custom_state) {}

    void gui_tick(ImGuiContext& gui) override
    {
        currentTime = custom_state.t;
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

        if (custom_state.input_to_imgui)
        {
            console.Draw("Console");

        }
    }

    void gl_tick() override {

        auto& camera = custom_state.camController->getPawn();
        auto& controller = custom_state.camController;

        // Check for events
        custom_state.last_cam_pos = custom_state.camController->getPawn().getPosition();




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
        custom_state.delta_cam_pos = custom_state.last_cam_pos - camera.getPosition();


        // Update the Uniform Buffer of the camera
        custom_state.camController->getPawn().updateUBO();

        custom_state.brushes[0].draw();






        custom_state.t += (*getDeltaTime());
        custom_state.camController->getPawn().getUBO().updateData(sizeof(glm::mat4) * 2, sizeof(float), (const void*)&(custom_state.t));

        double d_dt = *getDeltaTime();
        float dt = (float)d_dt;

        custom_state.camController->getPawn().getUBO().updateData((sizeof(glm::mat4) * 2) + sizeof(float), sizeof(float), (const void*)&dt);

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


        // ignore these, the console isn't real right now
        //console.target_application = &(custom_state);
        //console.open = &(custom_state.input_to_imgui);



        // Initialized the camera / controller
        //custom_state.camController
        custom_state.camController = new level_editor_controller();
        custom_state.camController->init(getWindowDimensions(), getDeltaTime(), &(custom_state));
        custom_state.camController->getPawn().getUBO().init();

        // Bind it to slot 0
        custom_state.camController->getPawn().getUBO().bindBase(0);
        custom_state.camController->getPawn().setPosition({ 0, 5, 0 });


        custom_state.brushes.emplace_back();
        brush_setup();
        custom_state.brushes[0].prepare_for_draw();




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


};