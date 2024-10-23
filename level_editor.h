#pragma once
#include "windowContent.h"
#include <windows.h>
#include <algorithm>
#include "Console.h"
#include "ent.h"
#include <SDL.h>
#include "glm_master.h"
#include "client.h"

extern Globals globals;

void FocusConsoleWindow() {
    // Functionality to focus on console window can be implemented here
}



void FocusSDLWindow(SDL_Window* sdlWindow) {
    // Bring SDL window to the foreground
    SDL_RaiseWindow(sdlWindow);
}

class level_editor_app : public Application {
private:
    // Any private member variables can go here

public:
    SDL_Event events;
    std::string fps_counter;
    float timeAccumulator = 0.0f;
    Console console;
    float previous_time = 0;
    //double currentTime = 0;
    float elapsed_time = 0.0;
    vec3 last_cam_pos;
    vec3 delta_cam_pos;
    bool input_to_imgui = 0;
    ent_container<ent_brush>& brushes = globals.server->get_entity_container<ent_brush>();
    //std::vector<ent_brush> ;
    

    level_editor_app(int argc, char** argv): Application()//,console(custom_state)
    {
        console.open = &input_to_imgui;
    }

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
            console.Draw();
        }
    }

    void gl_tick() override {

        // Check for events
        last_cam_pos = globals.current_client->camera->get_position();

        if (!input_to_imgui) {
            globals.current_client->controller.CheckContinuousInput();
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
                globals.current_client->controller.inputEvent(events);
            //}
            //else {
            //    ImGui_ImplSDL2_ProcessEvent(&events);
            //}
        }

        delta_cam_pos = last_cam_pos - globals.current_client->camera->get_position();

        // Update the Uniform Buffer of the globals.current_client->camera
        globals.current_client->camera->update_UBO();

        // Draw the brushes
        for (auto& brush : brushes) {
            brush.draw();
        }

        // Update time and delta time
        globals.t += getDeltaTime();
        //globals.current_client->camera->getUBO().set(offsetof(gl_globals, t), globals.t);


        //globals.current_client->camera->getUBO().set(offsetof(gl_globals, dt), globals.dt);

        globals.t += globals.dt; // Update global time
    }

    void bootstrap() override {
        brush_setup();
        //globals.current_client->camera = &camera;
        //globals.current_client->controller.mControlledEntity = &camera;

        //console.target_application = &(custom_state);
        //console.open = &(input_to_imgui);

        // Initialized the globals.current_client->camera / controller
        //camController = new level_editor_controller();
        globals.current_client->camera->getUBO().init();

        // Bind it to slot 0
        globals.current_client->camera->getUBO().bindBase(0);
        globals.current_client->camera->set_position({ 10, 10, 0 });
        globals.current_client->camera->look_at(vec3(0, 0, 0));

        // Prepare the first brush
        brushes.emplace_back(vec3(0.0,0.0,0.0), vec3(1.0,1.0,1.0));
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
