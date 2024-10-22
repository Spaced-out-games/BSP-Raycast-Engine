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

class level_editor_app : public Application<windowContent> {
private:
    // Any private member variables can go here

public:
    SDL_Event events;
    std::string fps_counter;
    float timeAccumulator = 0.0f;
    basic_console console;
    double previous_time = 0;
    double currentTime = 0;
    double elapsed_time = 0.0;

    level_editor_app(int argc, char** argv)
        : Application<windowContent>(), console(custom_state)
    {}

    void gui_tick(ImGuiContext& gui) override {
        currentTime = custom_state.t;
        elapsed_time = currentTime - previous_time;
        float& dt = globals.dt;

        if (elapsed_time >= 0.05) { // 0.05 seconds corresponds to 20 updates per second
            double fps = 1.0 / dt;  // Calculate FPS if dt > 0
            fps_counter = "FPS: " + std::to_string((int)fps);  // Update the FPS counter
            previous_time = currentTime;  // Update the previous time
        }

        if (dt > 0.0f) {
            ImGui::GetBackgroundDrawList()->AddText(NULL, 50.0f, ImVec2(30, 30), IM_COL32(255, 255, 255, 255), fps_counter.c_str());
        }

        if (custom_state.input_to_imgui) {
            console.Draw("Console");
        }
    }

    void gl_tick() override {
        auto& camera = custom_state.camController->camera;

        // Check for events
        custom_state.last_cam_pos = custom_state.camController->camera.get_position();

        if (!custom_state.input_to_imgui) {
            custom_state.camController->CheckContinuousInput();
        }

        while (SDL_PollEvent(&events)) {
            if (events.type == SDL_KEYDOWN && events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                // Toggle input to/from ImGui
                custom_state.input_to_imgui = !custom_state.input_to_imgui;

                // Toggle mouse cursor and capture state based on input_to_imgui
                if (custom_state.input_to_imgui) {
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
            if (!custom_state.input_to_imgui) {
                custom_state.camController->inputEvent(events);
            }
            else {
                ImGui_ImplSDL2_ProcessEvent(&events);
            }
        }

        custom_state.delta_cam_pos = custom_state.last_cam_pos - camera.get_position();

        // Update the Uniform Buffer of the camera
        custom_state.camController->camera.update_UBO();

        // Draw the brushes
        for (auto& brush : custom_state.brushes) {
            brush.draw();
        }

        // Update time and delta time
        custom_state.t += getDeltaTime();
        custom_state.camController->camera.getUBO().set(offsetof(gl_globals, t), custom_state.t);

        double d_dt = getDeltaTime();
        float dt = static_cast<float>(d_dt);
        custom_state.camController->camera.getUBO().set(offsetof(gl_globals, dt), dt);

        globals.t += globals.dt; // Update global time
    }

    void bootstrap() override {
        brush_setup();

        console.target_application = &(custom_state);
        console.open = &(custom_state.input_to_imgui);

        // Initialized the camera / controller
        custom_state.camController = new level_editor_controller();
        custom_state.camController->camera.getUBO().init();

        // Bind it to slot 0
        custom_state.camController->camera.getUBO().bindBase(0);
        custom_state.camController->camera.set_position({ 10, 0, 0 });
        custom_state.camController->camera.look_at(vec3(0, 0, 0));

        // Prepare the first brush
        custom_state.brushes.emplace_back();
        custom_state.brushes[0].prepare_for_draw();

        // Example for dynamic casting
        ent_camera e;
        ent_3d_2a* basePtr = &e;

        if (ent_3d_2a* downcastedPtr = dynamic_cast<ent_3d_2a*>(basePtr)) {
            std::cout << downcastedPtr->get_name() << std::endl; // Outputs: ent_camera
        }

        // Additional initialization can go here
    }

    void draw_brushes() {
        size_t initial_size = custom_state.brushes.size();
        auto& brushes = custom_state.brushes;

        // Loop through existing brushes
        for (size_t i = 0; i < initial_size; ++i) {
            brushes[i].draw();
        }
    }

    // Add additional functionality as needed
};
