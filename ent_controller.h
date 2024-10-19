#pragma once
#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include "GL\glew.h"
#include "Application.h"
#include "ent.h"
#include "movable_ent.h"

IMPORT_GLOBALS



class ent_controller : public ent {
public:
    ent_controller(ent_3d_2a& controlledEntity) : mControlledEntity(controlledEntity) {}
    ~ent_controller() {}

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

    glm::vec2 getMousePosition();
    glm::vec2 getNormalizedMousePos();

    glm::vec2& windowDimensions = globals.window_dimensions; // Assuming 'globals' is defined elsewhere
    glm::uvec2 mousePosition;

private:
    SDL_Event events;
    bool isPollingEvents = false; // Initialize polling events flag to false
    ent_3d_2a& mControlledEntity; // Reference to the controlled entity
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

glm::vec2 ent_controller::getMousePosition() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

glm::vec2 ent_controller::getNormalizedMousePos() {
    return ((getMousePosition() / windowDimensions) - glm::vec2(0.5f, 0.5f)) * glm::vec2(2.0f, 2.0f);
}

