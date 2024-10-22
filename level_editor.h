#pragma once
#include "windowContent.h"
#include <windows.h>
#include <algorithm>
#include "Console.h"
#include "ent.h"
#include <SDL.h>
#include "glm_master.h"

IMPORT_GLOBALS

void FocusConsoleWindow() {
    // Functionality to focus on console window can be implemented here
}



void FocusSDLWindow(SDL_Window* sdlWindow) {
    // Bring SDL window to the foreground
    SDL_RaiseWindow(sdlWindow);
}

class level_editor_app : public Application<char> {
private:
    // Any private member variables can go here

public:
    SDL_Event events;
    std::string fps_counter;
    float timeAccumulator = 0.0f;
    //basic_console console;
    float previous_time = 0;
    //double currentTime = 0;
    float elapsed_time = 0.0;
    level_editor_controller controller;
    ent_camera camera;
    vec3 last_cam_pos;
    vec3 delta_cam_pos;
    bool input_to_imgui = 0;
    std::vector<ent_brush> brushes;

    level_editor_app(int argc, char** argv)
        : Application<char>(), controller(&camera)//,console(custom_state)
    {}

    void gui_tick(ImGuiContext& gui) override {
        
        elapsed_time = globals.t - previous_time;
        float& dt = globals.dt;

        if (elapsed_time >= 0.05) { // 0.05 seconds corresponds to 20 updates per second
            double fps = 1.0 / dt;  // Calculate FPS if dt > 0
            fps_counter = "FPS: " + std::to_string((int)fps);  // Update the FPS counter
            previous_time = globals.t;  // Update the previous time
        }

        if (dt > 0.0f) {
            ImGui::GetBackgroundDrawList()->AddText(NULL, 50.0f, ImVec2(30, 30), IM_COL32(255, 255, 255, 255), fps_counter.c_str());
        }

        if (input_to_imgui) {
            //console.Draw("Console");
        }
    }

    void gl_tick() override {

        // Check for events
        last_cam_pos = camera.get_position();

        if (!input_to_imgui) {
            controller.CheckContinuousInput();
        }

        while (SDL_PollEvent(&events)) {
            if (events.type == SDL_KEYDOWN && events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                // Toggle input to/from ImGui
                input_to_imgui = !input_to_imgui;

                // Toggle mouse cursor and capture state based on input_to_imgui
                if (input_to_imgui) {
                    // Show the mouse cursor and release it
                    SDL_ShowCursor(SDL_ENABLE);
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
                else {
                    // Hide the mouse cursor and capture it
                    SDL_ShowCursor(SDL_DISABLE);
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }

            // Pass the event to the Controller
            //if (!input_to_imgui) {
                controller.inputEvent(events);
            //}
            //else {
            //    ImGui_ImplSDL2_ProcessEvent(&events);
            //}
        }

        delta_cam_pos = last_cam_pos - camera.get_position();

        // Update the Uniform Buffer of the camera
        camera.update_UBO();

        // Draw the brushes
        for (auto& brush : brushes) {
            brush.draw();
        }

        // Update time and delta time
        globals.t += getDeltaTime();
        //camera.getUBO().set(offsetof(gl_globals, t), globals.t);


        //camera.getUBO().set(offsetof(gl_globals, dt), globals.dt);

        globals.t += globals.dt; // Update global time
    }

    void bootstrap() override {
        brush_setup();


        //console.target_application = &(custom_state);
        //console.open = &(input_to_imgui);

        // Initialized the camera / controller
        //camController = new level_editor_controller();
        camera.getUBO().init();

        // Bind it to slot 0
        camera.getUBO().bindBase(0);
        camera.set_position({ 10, 10, 0 });
        camera.look_at(vec3(0, 0, 0));

        // Prepare the first brush
        brushes.emplace_back();
        brushes[0].prepare_for_draw();

        // Example for dynamic casting


        // Additional initialization can go here
    }

    void draw_brushes() {
        size_t initial_size = brushes.size();

        // Loop through existing brushes
        for (size_t i = 0; i < initial_size; ++i) {
            brushes[i].draw();
        }
    }

    // Add additional functionality as needed
};
