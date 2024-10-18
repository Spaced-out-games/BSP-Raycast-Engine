#pragma once
#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include "GL/glew.h"

#include "server_definition.h"
#include "server.h"
#include "Camera.h"
extern server_t& server;

// Define ent_controller class

// Pretty much a Controller but wrapped as an ent
class ent_controller : public ent
{
public:

    struct
    {
        float current_speed = 10.0f;// Now in `ControllerState`
        float vertical_sensitivity = 50.0f; // Now in `ControllerState`
        float horizontal_sensitivity = 50.0f;// Now in `ControllerState`
    };


    ent_camera& camera;
    ent_controller(size_t index): camera(server.get_container<ent_camera>().get(index)) {};
    ~ent_controller();

    void input_event();
    SDL_Event& get_event() { return mEvent; }

    // Poll for the next event
    int get_next_event()
    {
        return SDL_PollEvent(&mEvent);
    }
    glm::ivec2 window_dimensions;

    virtual void onQuit() {};
    virtual void onKeyDown() {};
    virtual void onKeyUp() {};
    virtual void onMouseMove() {};
    virtual void onMouseButtonDown() {};
    virtual void onMouseButtonUp() {};
    virtual void onMouseWheelMove() {};
    virtual void onFileDrop() {};
    virtual void onTextDrop() {};
    virtual void onBeginDrop() {};
    virtual void onEndDrop() {};
    void CheckContinuousInput() {
        deltaMousePos =get_mouse_position_normalized() - lastMousePos;
        state = SDL_GetKeyboardState(nullptr);
        float dt = server.dt;
        

        if (state[SDL_SCANCODE_W]) {
            camera.moveForward(current_speed * (dt));
        }
        if (state[SDL_SCANCODE_S]) {
            camera.moveForward(-current_speed * (dt));
        }
        if (state[SDL_SCANCODE_A]) {
            camera.moveRight(current_speed * (dt));
        }
        if (state[SDL_SCANCODE_D]) {
            camera.moveRight(-current_speed * (dt));
        }

        glm::vec2& mouse = get_mouse_position_normalized();
        if ((mouse.x < -0.5f || mouse.x > 0.5f || mouse.y < -0.5f || mouse.y > 0.5f)) {
            SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), window_dimensions.x / 2, window_dimensions.y / 2);
        }
        camera.rotate(deltaMousePos.y * glm::radians(90.0) * vertical_sensitivity,
            -deltaMousePos.x * glm::radians(90.0) * horizontal_sensitivity);

        lastMousePos = get_mouse_position_normalized();
    }
    virtual void onWindowResize()
    {
        SDL_Event& event = get_event();
        glViewport(0, 0, event.window.data1, event.window.data2);

        // Update window widths and heights when the window is resized
        window_dimensions = { static_cast<unsigned int>(event.window.data1), static_cast<unsigned int>(event.window.data2) };
    }

    void update_mouse_position();
    glm::ivec2 get_mouse_position();
    glm::vec2 get_mouse_position_normalized();
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    glm::vec2 normalize_to_screen_space(glm::ivec2 screen_position);
    glm::vec2 lastMousePos = { 0.0, 0.0 }; // Now in `ControllerState`
    glm::vec2 deltaMousePos = { 0.0, 0.0 }; // Now in `ControllerState`
    glm::ivec2* window_dimensions_ptr;

    // Unnormalized mouse position
    glm::ivec2 mouse_position;
    //ent_camera* camera;

private:
    SDL_Event mEvent;
};

// Constructors


// Destructor
ent_controller::~ent_controller() {}

// input decision tree
void ent_controller::input_event()
{
    switch (mEvent.type) {
    case SDL_QUIT:
        onQuit();
        break;
    case SDL_KEYUP:
        onKeyUp();
        break;
    case SDL_KEYDOWN:
        onKeyDown();
        break;
    case SDL_MOUSEMOTION:
        onMouseMove();
        break;
    case SDL_MOUSEBUTTONDOWN:
        onMouseButtonDown();
        break;
    case SDL_MOUSEBUTTONUP:
        onMouseButtonUp();
        break;
    case SDL_MOUSEWHEEL:
        onMouseWheelMove();
        break;
    case SDL_DROPFILE:
        onFileDrop();
        break;
    case SDL_DROPTEXT:
        onTextDrop();
        break;
    case SDL_DROPBEGIN:
        onBeginDrop();
        break;
    case SDL_DROPCOMPLETE:
        onEndDrop();
        break;
    case SDL_WINDOWEVENT:
        if (mEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
            onWindowResize();
            std::cout << "Window resized\n";
        }
        break;
    case SDL_APP_WILLENTERBACKGROUND:
        std::cout << "App will enter background\n";
        break;
    case SDL_APP_DIDENTERBACKGROUND:
        std::cout << "App entered background\n";
        break;
    case SDL_APP_WILLENTERFOREGROUND:
        std::cout << "App will enter foreground\n";
        break;
    case SDL_APP_DIDENTERFOREGROUND:
        std::cout << "App entered foreground\n";
        break;
    default:
        break;
    }
}

void ent_controller::update_mouse_position()
{
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
}

glm::ivec2 ent_controller::get_mouse_position()
{
    return mouse_position;
}

glm::vec2 ent_controller::normalize_to_screen_space(glm::ivec2 screen_position)
{
    glm::vec2 screen_position_float(static_cast<float>(screen_position.x), static_cast<float>(screen_position.y));

    return ((screen_position_float / glm::vec2(window_dimensions)) - glm::vec2(0.5f, 0.5f)) * glm::vec2(2.0f, 2.0f);


}

glm::vec2 ent_controller::get_mouse_position_normalized()
{
    return normalize_to_screen_space(mouse_position);
}



