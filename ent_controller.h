#pragma once
#include <SDL.h>
#include <iostream>
#include "glm_master.h"
#include "GL\glew.h"
#include "Application.h"
#include "ent.h"
#include "movable_ent.h"
//#include "globals.h"
extern Globals globals;







class ent_controller : public ent {
public:
    // Default constructor (without controlled entity)
    ent_controller() : mControlledEntity(nullptr), lastMousePosition{ 0, 0 } {
        // Initialize any default settings or values here if needed
    }

    // Constructor that takes a pointer to an ent_3d_2a entity
    ent_controller(ent_3d_2a* controlledEntity) : mControlledEntity(controlledEntity), lastMousePosition{ 0, 0 } {}



    ~ent_controller() {}

    const char* get_name() const override
    {
        return "ent_controller";
    }

    void inputEvent(SDL_Event& events);
    void enablePollingEvents();
    void disablePollingEvents();
    SDL_Event& getEvents() { return events; }

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
    virtual void onWindowResize(SDL_Event event) {
        glViewport(0, 0, event.window.data1, event.window.data2);
        // Update window dimensions when the window is resized
        windowDimensions = { static_cast<unsigned int>(event.window.data1), static_cast<unsigned int>(event.window.data2) };
    }
    // Copy assignment operator
    ent_controller& operator=(const ent_controller& other) {
        if (this != &other) {  // Check for self-assignment
            // Copy basic data members
            events = other.events;
            isPollingEvents = other.isPollingEvents;
            lastMousePosition = other.lastMousePosition;
            mousePosition = other.mousePosition;

            // Copy the pointer (shallow copy)
            mControlledEntity = other.mControlledEntity;  // Shallow copy, decide if you need a deep copy.

            // windowDimensions is a reference to globals.window_dimensions, so no reassignment is needed.
        }
        return *this;
    }
    vec2 getMousePosition();
    vec2 getNormalizedMousePos();
    vec2 get_delta_mouse_pos();
    vec2 get_delta_mouse_pos_normalized();
    vec2 get_mouse_pos();
    vec2 get_mouse_pos_normalized();

    vec2& windowDimensions = globals.window_dimensions; // Assuming 'globals' is defined elsewhere
    glm::uvec2 mousePosition;
    ent_3d_2a* mControlledEntity; // Pointer to the controlled entity

private:
    SDL_Event events;
    bool isPollingEvents = false; // Initialize polling events flag to false
    vec2 lastMousePosition; // Store the last mouse position for delta calculations
};

void ent_controller::inputEvent(SDL_Event& events) {
    if (!isPollingEvents) return;

    switch (events.type) {
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
        lastMousePosition = getMousePosition(); // Update last mouse position on mouse move
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

void ent_controller::enablePollingEvents() {
    isPollingEvents = true;
}

void ent_controller::disablePollingEvents() {
    isPollingEvents = false;
}

vec2 ent_controller::getMousePosition() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return vec2(static_cast<float>(x), static_cast<float>(y));
}

vec2 ent_controller::getNormalizedMousePos() {
    return ((getMousePosition() / windowDimensions) - vec2(0.5f, 0.5f)) * vec2(2.0f, 2.0f);
}

vec2 ent_controller::get_delta_mouse_pos() {
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    return vec2(static_cast<float>(x), static_cast<float>(y));
}

vec2 ent_controller::get_delta_mouse_pos_normalized() {
    return get_delta_mouse_pos() / windowDimensions; // Normalizing relative mouse delta
}

vec2 ent_controller::get_mouse_pos() {
    return getMousePosition(); // Return current mouse position
}

vec2 ent_controller::get_mouse_pos_normalized() {
    return getNormalizedMousePos(); // Return normalized current mouse position
}


