#pragma once
#include <windows.h>
#include <algorithm>
#include "Console.h"
#include "entity.h"
#include "client.h"

void FocusConsoleWindow() {

}
void FocusSDLWindow(SDL_Window* sdlWindow) {
    // Bring SDL window to the foreground
    SDL_RaiseWindow(sdlWindow);
}

class level_editor_app: public ent, Client<char>, Application<char>
{
    public:
        std::string fps_counter;
        float previous_time = 0.0;
        float elapsed_time = 0.0;
        //basic_console console;

        void run() { Application<char>::run(); }
        level_editor_app() {
            deltaTime = &dt;
        };
        ~level_editor_app() = default;

        virtual void draw() override {};
        virtual void tick() override {};
        virtual void init() override {};
        virtual void exec(ent_request stream) override {};


        void gui_tick(ImGuiContext& gui) override
        {
            
            elapsed_time = t - previous_time;


            if (elapsed_time >= 0.05)  // 0.05 seconds corresponds to 20 updates per second
            {
                float fps = 1.0 / dt;  // Calculate FPS if dt > 0
                fps_counter = "FPS: " + std::to_string((int)fps);  // Update the FPS counter
                previous_time = t;  // Update the previous time
            }
            if (dt > 0.0f)
            {
                ImGui::GetBackgroundDrawList()->AddText(NULL, 50.0f, ImVec2(30, 30), IM_COL32(255, 255, 255, 255), fps_counter.c_str());
            }
            
            if (input_mode == 1)
            {
                //console.Draw("Console");
            }
        }

        void gl_tick() override {
            auto& camera = *(Client<char>::camera);
            auto& controller = *(Client<char>::controller);

            camera.last_cam_pos = camera.getPosition();

            if (input_mode == 0)
            {
                controller.CheckContinuousInput();
            }
            SDL_Event& events = controller.get_event();
            while (controller.get_next_event())
            {
                if (events.type == SDL_KEYDOWN && events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    input_mode ^= 1;
                }

                if (input_mode == 0)
                {
                    std::cout << "t";
                    controller.input_event();
                }
                else
                {
                    ImGui_ImplSDL2_ProcessEvent(&events);
                }
                camera.delta_cam_pos = camera.last_cam_pos - camera.getPosition();

                camera.updateUBO();

                // custom_state.brushes[0].draw();
                draw_brushes();
                
                // update the time uniform
                camera.getUBO().updateData(sizeof(glm::mat4) * 2, sizeof(float), (const void*)&t);
                camera.getUBO().updateData((sizeof(glm::mat4) * 2) + sizeof(float), sizeof(float), (const void*)&dt);
            }
        }

        void bootstrap() override {
            //Client<char>::init();

            // needs implemented properly probably
            controller->init();

            camera->getUBO().init();
            camera->getUBO().bindBase(0);
            //camera->setPosition({ 0, 5, 0 });
            //camera->lookAt({ 0,0,0 });

            server.add_entity<ent_brush>();
            //brush_setup();
            

        }

        void draw_brushes()
        {
            ent_container<ent_brush>& brushes = server.get_container<ent_brush>();
            size_t initial_size = brushes.size();
            for (size_t i = 0; i < initial_size; i++)
            {
                brushes.get(i).draw();
            }
        }

    private:

        

};