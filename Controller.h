#pragma once
#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include "GL\glew.h"



template <class Pawn>
class Controller
{
public:
    Controller(glm::uvec2* windowDimensions_ptr);
    Controller() = default;
    ~Controller();

    void inputEvent(SDL_Event& events);
    void enablePollingEvents();
    void disablePollingEvents();
    SDL_Event& getEvents() { return events; }

    virtual void onQuit(SDL_Event event) {};
    virtual void onKeyDown(SDL_Event event) {};
    virtual void onKeyUp(SDL_Event event) {};
    virtual void onMouseMove(SDL_Event event) {  };
    virtual void onMouseButtonDown(SDL_Event event) {};
    virtual void onMouseButtonUp(SDL_Event event) {};
    virtual void onMouseWheelMove(SDL_Event event) {};
    virtual void onFileDrop(SDL_Event event) {};
    virtual void onTextDrop(SDL_Event event) {};
    virtual void onBeginDrop(SDL_Event event) {};
    virtual void onEndDrop(SDL_Event event) {};
    virtual void onWindowResize(SDL_Event event) {
        glViewport(0, 0, event.window.data1, event.window.data2);

        // Update window widths and heights when the window is resized
        *windowDimensions = { static_cast<unsigned int>(event.window.data1), static_cast<unsigned int>(event.window.data2) };
        //windowHeight = ;

    };
    //SDL_Event events;


    glm::vec2 getMousePosition();
    // Returns mouse position in(-1.0 - 1.0) range in window space
    glm::vec2 getNormalizedMousePos();
    Pawn& getPawn() { return mPawn; }
    //glm::vec2 getWindowDimensions() { return (*windowDimensions); }
    glm::vec2* windowDimensions;
    glm::uvec2 mousePosition;

private:
    // Using floats since they are more precise to scale

    Pawn mPawn;
    bool isPollingEvents;
};

// Define the template member functions in the same header file
template <class Pawn>
Controller<Pawn>::Controller(glm::uvec2* windowDimensions_ptr) : isPollingEvents(false), mPawn(pawn), windowDimensions(windowDimensions_ptr)
{
}

template <class Pawn>
Controller<Pawn>::~Controller() {}

template <class Pawn>
void Controller<Pawn>::inputEvent(SDL_Event& events) {
    //while (SDL_PollEvent(&events)) {
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
        //}
    }
}

template <class Pawn>
void Controller<Pawn>::enablePollingEvents() {
    isPollingEvents = true;
}

template <class Pawn>
void Controller<Pawn>::disablePollingEvents() {
    isPollingEvents = false;
}

template <class Pawn>
glm::vec2 Controller<Pawn>::getMousePosition() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}
template <class Pawn>
glm::vec2 Controller<Pawn>::getNormalizedMousePos()
{


    return ((getMousePosition() / (*windowDimensions)) - glm::vec2(0.5, 0.5)) * glm::vec2(2.0, 2.0);// getMousePosition();
}
