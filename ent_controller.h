#pragma once
#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include "GL/glew.h"

#include "server_definition.h"
#include "server.h"
#include "Camera.h"
extern server_t& server;

// Define ControllerState struct
template <typename T = char >
struct ControllerState
{
    glm::uvec2 window_dimensions; // Store window dimensions
    SDL_Event event; // SDL event
    glm::vec2 mouse_pos; // Current mouse position
    glm::vec2 delta_mouse_pos; // Change in mouse position
    glm::vec2 last_mouse_pos;
    float deltaTime;
    float vertical_sensitivity = 50.0f;
    float horizontal_sensitivity = 50.0f;
    float move_speed = 10.0;
    float sprint_multiplier = 2.0f;
    float current_speed = 10.0f;

    // Poll for the next event
    int get_next_event()
    {
        return SDL_PollEvent(&event);
    }

    // Get the current mouse position
    glm::vec2 get_mouse_position()
    {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        return glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
    }

    // Update mouse position
    void mouse_tick()
    {
        last_mouse_pos = mouse_pos;
        glm::vec2 new_position = ((get_mouse_position() / glm::vec2(window_dimensions)) - glm::vec2(0.5, 0.5)) * glm::vec2(2.0, 2.0);
        delta_mouse_pos = new_position - mouse_pos;
        mouse_pos = new_position;
    }

    inline glm::vec2 get_normalized_mouse_position() { return mouse_pos; }

    T custom_controller_state; // Optional custom state
};

// Define ent_controller class

class ent_controller : public ent
{
    const Uint8* state;
    ControllerState<>* game_state_ptr; // Pointer to game state
public:
    bool marked_for_processing;
    ent_camera* camera = nullptr;

    ent_controller(ControllerState<>* game_state);
    ent_controller();
    ~ent_controller() = default;

    // Poll for the next event
    int get_next_event()
    {
        return game_state_ptr->get_next_event();
    }
    SDL_Event& get_current_event() { return game_state_ptr->event; }

    void input_event(); // Handle input events
    ControllerState<>& get_game_state() { return *game_state_ptr; }

    // Event handlers
    virtual void onQuit(SDL_Event event) {};
    virtual void onKeyDown(SDL_Event event) {};
    virtual void onKeyUp(SDL_Event event) {};
    virtual void onMouseMove(SDL_Event event) {};
    virtual void onMouseButtonDown(SDL_Event event) {};
    virtual void onMouseButtonUp(SDL_Event event) {};
    virtual void onMouseWheelMove(SDL_Event event) {};
    virtual void onFileDrop(SDL_Event event) {};
    virtual void onTextDrop(SDL_Event event) {};
    virtual void onBeginDrop(SDL_Event event) {};
    virtual void onEndDrop(SDL_Event event) {};
    virtual void onWindowResize(SDL_Event event);

    inline glm::vec2 get_mouse_position();
    inline glm::vec2 get_normalized_mouse_position();
    void CheckContinuousInput() {
        glm::vec2& deltaMousePos = get_normalized_mouse_position() - game_state_ptr->last_mouse_pos;
        
        float& dt = game_state_ptr->deltaTime;
        ent_camera& target_camera = *camera;
        //Camera& target_camera = getPawn();

        if (state[SDL_SCANCODE_W]) {
            target_camera.moveForward(game_state_ptr->current_speed * (game_state_ptr->deltaTime));
        }
        if (state[SDL_SCANCODE_S]) {
            target_camera.moveForward(-game_state_ptr->current_speed * (game_state_ptr->deltaTime));
        }
        if (state[SDL_SCANCODE_A]) {
            target_camera.moveRight(game_state_ptr->current_speed * (game_state_ptr->deltaTime));
        }
        if (state[SDL_SCANCODE_D]) {
            target_camera.moveRight(-game_state_ptr->current_speed * (game_state_ptr->deltaTime));
        }

        glm::vec2& mouse = get_normalized_mouse_position();
        if ((mouse.x < -0.5f || mouse.x > 0.5f || mouse.y < -0.5f || mouse.y > 0.5f)) {
            SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), game_state_ptr->window_dimensions.x / 2, game_state_ptr->window_dimensions.y / 2);
        }
        
        target_camera.rotate(game_state_ptr->delta_mouse_pos.y * glm::radians(90.0f) * game_state_ptr->vertical_sensitivity,
            -game_state_ptr->delta_mouse_pos.x * glm::radians(90.0f) * game_state_ptr->horizontal_sensitivity);


        game_state_ptr->last_mouse_pos = get_normalized_mouse_position();
    }
};

// Constructor for ent_controller. Needs cleaned up
ent_controller::ent_controller(ControllerState<>* game_state) : game_state_ptr(game_state)

{
    state = SDL_GetKeyboardState(nullptr);
}

// Constructor for ent_controller
ent_controller::ent_controller()

{
    game_state_ptr = new ControllerState<>();
    // get the index of the next one to be added
    //size_t index = server.get_container<ent_camera>().size();
    // Add a camera to the scene
   // server.get_container<ent_camera>().add();
    size_t index = server.get_container<ent_camera>().size();
    // Add a camera to the scene
    server.get_container<ent_camera>().add();

    // Assign the camera
    camera = &(server.get_container<ent_camera>().get(index));
    // Assign the camera
    //camera = &server.get_container<ent_camera>().get(index);
    state = SDL_GetKeyboardState(nullptr);
}
//ent_controller::ent_controller (ent_camera&)... belongs here

// Input event handler
void ent_controller::input_event()
{
    game_state_ptr->get_next_event(); // Get the next SDL event
    SDL_Event& events = game_state_ptr->event; // Reference to the current event

    switch (events.type)
    {
    case SDL_QUIT:
        onQuit(events);
        break;
    case SDL_KEYUP:
        onKeyUp(events);
        break;
    case SDL_KEYDOWN:
        onKeyDown(events);
        break;
    case SDL_MOUSEMOTION:
        onMouseMove(events);
        break;
    case SDL_MOUSEBUTTONDOWN:
        onMouseButtonDown(events);
        break;
    case SDL_MOUSEBUTTONUP:
        onMouseButtonUp(events);
        break;
    case SDL_MOUSEWHEEL:
        onMouseWheelMove(events);
        break;
    case SDL_DROPFILE:
        onFileDrop(events);
        break;
    case SDL_DROPTEXT:
        onTextDrop(events);
        break;
    case SDL_DROPBEGIN:
        onBeginDrop(events);
        break;
    case SDL_DROPCOMPLETE:
        onEndDrop(events);
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
    case SDL_WINDOWEVENT_RESIZED:
        onWindowResize(events);
        std::cout << "Window resized\n";
        break;
    default:
        break;
    }
}

// Window resize event handler
void ent_controller::onWindowResize(SDL_Event event)
{
    glViewport(0, 0, event.window.data1, event.window.data2);
    game_state_ptr->window_dimensions = { static_cast<unsigned int>(event.window.data1), static_cast<unsigned int>(event.window.data2) };
}

// Get current mouse position
glm::vec2 ent_controller::get_mouse_position()
{
    return game_state_ptr->get_mouse_position();
}

// Get normalized mouse position
glm::vec2 ent_controller::get_normalized_mouse_position()
{
    return game_state_ptr->get_normalized_mouse_position();
}








/*
class ent_level_editor_controller : public ent_controller {
public:
    ent_level_editor_controller(ControllerState<>* controller_ptr, ent_reference camera_reference) : ent_controller(controller_ptr), camera_reference(camera_reference)
    {
        camera_reference = server.add_entity<ent_camera>(ent_camera());
        server.get_container_by_reference<ent_camera>(camera_reference).
    };

    bool clip = false;
    // reference to the camera.
    ent_reference camera_reference;
    
    bool receiving_input = false;
    bool marked_for_processing = false;
    std::string commands;
    // Instead of a windowContent*, juse use the server
};
*/