#pragma once

#include "ent_camera.h"
#include "ent_controller.h"
#include <SDL.h>
#include "debug_utils.h"

// Forward declaration of windowContent
//struct windowContent;

class level_editor_controller : public ent_controller {
public:
    level_editor_controller();
    level_editor_controller(ent_3d_2a* controlledEntity) : ent_controller(controlledEntity) {}
    void onMouseMove(SDL_Event event) override;
    void CheckContinuousInput();
    void onKeyDown(SDL_Event event) override;
    void onKeyUp(SDL_Event event) override;
    void init() override;
    void tick() override {};
    void exec(const ent_command& command) override {};


    float& deltaTime = globals.dt;
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    glm::vec2 lastMousePos = { 0.0, 0.0 };
    glm::vec2 deltaMousePos = { 0.0, 0.0 };
    float vertical_sensitivity = 25.0f;
    float horizontal_sensitivity = 25.0f;
    float move_speed = 10.0;
    float sprint_multiplier = 2.0f;
    float current_speed = 10.0f;
    glm::vec3 last_camera_pos;
    glm::vec3 delta_camera_pos;
    bool clip = false;
    bool receiving_input = false;
    bool marked_for_processing = false;
    std::string commands;
    //windowContent* window_content;  // Pointer to windowContent to avoid circular dependency
    //ent_camera camera;
};

#include "level_editor_controller.h"
#include "windowContent.h"

level_editor_controller::level_editor_controller() {
}

void level_editor_controller::init(){
    //windowDimensions = window_width_ptr;
    //window_content = window_content_ptr;
}

//void level_editor_controller::init() override
//{

//}

void level_editor_controller::onMouseMove(SDL_Event event) {}

void level_editor_controller::CheckContinuousInput() {
    // Calculate the difference in mouse position
    deltaMousePos = getNormalizedMousePos() - lastMousePos;


    // Get the current state of the keyboard
    state = SDL_GetKeyboardState(nullptr);
    ent_3d_2a& target_object = *mControlledEntity;

    // Handle camera movement based on keyboard input
    if (state[SDL_SCANCODE_W]) {
        target_object.move_forward(current_speed * deltaTime);
    }
    if (state[SDL_SCANCODE_S]) {
        target_object.move_forward(-current_speed * deltaTime);
    }
    if (state[SDL_SCANCODE_A]) {
        target_object.move_right(-current_speed * deltaTime);
    }
    if (state[SDL_SCANCODE_D]) {
        target_object.move_right(current_speed * deltaTime);
    }

    // Normalize mouse position and check bounds
    glm::vec2 mouse = getNormalizedMousePos();
    if (mouse.x < -0.5f || mouse.x > 0.5f || mouse.y < -0.5f || mouse.y > 0.5f) {
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), windowDimensions.x / 2, windowDimensions.y / 2);
    }

    // Calculate rotation based on mouse movement
    glm::vec2 delta = glm::vec2(deltaMousePos.y * glm::radians(90.0f) * vertical_sensitivity,
        -deltaMousePos.x * glm::radians(90.0f) * horizontal_sensitivity);
    target_object.rotate(delta);

    // Update lastMousePos for the next frame
    lastMousePos = getNormalizedMousePos();
}


void level_editor_controller::onKeyDown(SDL_Event event) {
    if (!receiving_input) {
        switch (event.key.keysym.sym) {
        //case SDLK_ESCAPE:
            //std::cout << "exiting game";
            //exit(32);
        case SDLK_LSHIFT:
            current_speed = move_speed * sprint_multiplier;
            break;
        default:
            break;
        }
    }
}

void level_editor_controller::onKeyUp(SDL_Event event) {
    switch (event.key.keysym.sym) {
    case SDLK_LSHIFT:
        current_speed = move_speed;
        break;
    default:
        break;
    }
}
