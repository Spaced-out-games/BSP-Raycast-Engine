#pragma once

#include "Camera.h"
#include "Controller.h"
#include <SDL.h>
#include "debug_utils.h"
//#include "ent_controller.h"

// Forward declaration of windowContent

struct windowContent;


class level_editor_controller : public Controller<Camera> {
public:
    level_editor_controller();
    void init(glm::vec2* window_width_ptr, double* deltaTime_ptr, windowContent* window_contentnt_ptr);
    void onMouseMove(SDL_Event event) override;
    void CheckContinuousInput();
    void onKeyDown(SDL_Event event) override;
    void onKeyUp(SDL_Event event) override;

    double* deltaTime; // Now in Client as `dt`
    const Uint8* state = SDL_GetKeyboardState(nullptr); // Now in ent_controller as `state`
    glm::vec2 lastMousePos = { 0.0, 0.0 }; // Now in `ControllerState`
    glm::vec2 deltaMousePos = { 0.0, 0.0 }; // Now in `ControllerState`
    float vertical_sensitivity = 50.0f; // Now in `ControllerState`
    float horizontal_sensitivity = 50.0f;// Now in `ControllerState`
    float move_speed = 10.0;// Now in `ControllerState`
    float sprint_multiplier = 2.0f;// Now in `ControllerState`
    float current_speed = 10.0f;// Now in `ControllerState`
    glm::vec3 last_camera_pos; // Now in `ent_camera`
    glm::vec3 delta_camera_pos; // Now in `ent_camera`
    bool clip = false;          // Now in `client'
    bool receiving_input = false;   // Now in `ent_controller`
    bool marked_for_processing = false; // Now in `ent_controller`
    std::string commands;               // Now in `Server` as `command_string`
    windowContent* window_content;      // Deprecated
};


level_editor_controller::level_editor_controller() {
    windowDimensions = nullptr;
    deltaTime = nullptr;
}

void level_editor_controller::init(glm::vec2* window_width_ptr, double* deltaTime_ptr, windowContent* window_content_ptr) {
    windowDimensions = window_width_ptr;
    deltaTime = deltaTime_ptr;
    window_content = window_content_ptr;
}

void level_editor_controller::onMouseMove(SDL_Event event) {}

void level_editor_controller::CheckContinuousInput() {
    deltaMousePos = getNormalizedMousePos() - lastMousePos;
    state = SDL_GetKeyboardState(nullptr);
    float dt = (float)(*deltaTime);
    Camera& target_camera = getPawn();

    if (state[SDL_SCANCODE_W]) {
        target_camera.moveForward(current_speed * (*deltaTime));
    }
    if (state[SDL_SCANCODE_S]) {
        target_camera.moveForward(-current_speed * (*deltaTime));
    }
    if (state[SDL_SCANCODE_A]) {
        target_camera.moveRight(current_speed * (*deltaTime));
    }
    if (state[SDL_SCANCODE_D]) {
        target_camera.moveRight(-current_speed * (*deltaTime));
    }

    glm::vec2& mouse = getNormalizedMousePos();
    if ((mouse.x < -0.5f || mouse.x > 0.5f || mouse.y < -0.5f || mouse.y > 0.5f)) {
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), windowDimensions->x / 2, windowDimensions->y / 2);
    }
    getPawn().rotate(deltaMousePos.y * glm::radians(90.0) * vertical_sensitivity,
        -deltaMousePos.x * glm::radians(90.0) * horizontal_sensitivity);

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
//*/