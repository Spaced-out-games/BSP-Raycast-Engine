#pragma once
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <chrono>

#ifndef APPLICATION_H
#define APPLICATION_H


// TODO:
// Make the variables private
// Make private setters that Application can alter through a proxy structure
// Make public const getters


struct Globals {
    float t; // Time
    float dt; // Delta time
    glm::vec2 window_dimensions; // Window dimensions

    // Private constructor to prevent instantiation
    Globals() : t(0.0f), dt(0.0f), window_dimensions(1920.0f, 1080.0f) {}


};

static Globals globals;


template <class state_t>
class Application
{
public:
    bool recenter_mouse = false;

    Application(int argc, char** argv);
    ~Application();

    // Quits the application
    void quit();

    // Returns whether the engine is currently running
    bool isRunning();

    // Main run loop
    int run();

    // Getters
    SDL_GLContext& getContext();
    static float& getDeltaTime() { return globals.dt; }
    static float& getTime() { return globals.t; }
    SDL_Window* getWindow();
    glm::vec2& get_window_dimensions() { return globals.window_dimensions; }

    void set_window_dimensions(glm::uvec2 new_dimensions);
    void setCustomState(const state_t& state);

    ImGuiContext* get_gui() const;

    // State management
    state_t& getCustomState();

    // Tick / bootstrapping methods
    virtual void bootstrap() { std::cout << "bootstrap\n"; }
    virtual void gl_tick() { std::cout << "tick\n"; }
    virtual void gui_tick(ImGuiContext& gui) {}

    state_t custom_state;
    
    static Globals& get_globals() {
        return globals;
    }


    
private:

    void init_SDL();
    ImGuiContext* imgui_context = nullptr;
    SDL_Window* sdlWindow;
    SDL_GLContext glContext;

    // Other members
    bool vsync_enabled;
    bool running;
    bool init_success;
    bool mFullscreen;
    std::string mTitle;
};

// Define the static member outside the class


template <class state_t>
Application<state_t>::Application(int argc, char** argv) :
    sdlWindow(nullptr),
    glContext(nullptr),
    vsync_enabled(false),
    running(false),
    init_success(false),
    mFullscreen(false),
    mTitle("Engine")
{
    #ifdef _DEBUG
    std::cout << "engineCore constructed\n";
    #endif
    init_SDL();
}

template <class state_t>
Application<state_t>::~Application()
{
    if (glContext)
    {
        SDL_GL_DeleteContext(glContext);
    }

    if (sdlWindow)
    {
        SDL_DestroyWindow(sdlWindow);
    }

    SDL_Quit();
}

template <class state_t>
ImGuiContext* Application<state_t>::get_gui() const { return imgui_context; }

template <class state_t>
void Application<state_t>::init_SDL()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Set OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    // Create SDL window
    sdlWindow = SDL_CreateWindow(mTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        globals.window_dimensions.x, globals.window_dimensions.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!sdlWindow) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    SDL_GL_SetSwapInterval(1); // Enable vsync
    glViewport(0, 0, globals.window_dimensions.x, globals.window_dimensions.y);
    glClearColor(0.0f, 0.4f, 0.5f, 1.0f);

    // Create ImGui context
    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    std::cout << "SDL, OpenGL, and ImGui successfully initialized.\n";
}

template <class state_t>
void Application<state_t>::quit()
{
    running = false;
}

template <class state_t>
bool Application<state_t>::isRunning()
{
    return running;
}

template <class state_t>
int Application<state_t>::run()
{
    bootstrap();

    #ifdef _DEBUG
    std::cout << "engineCore bootstrapped\n";
    #endif

    SDL_ShowCursor(SDL_DISABLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    running = true;

    while (running)
    {
        auto start = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl_tick();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        gui_tick((*imgui_context));

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(sdlWindow);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = end - start;
        globals.dt = elapsed.count();
    }
    return 0;
}

template <class state_t>
SDL_GLContext& Application<state_t>::getContext()
{
    return glContext;
}

template <class state_t>
void Application<state_t>::set_window_dimensions(glm::uvec2 new_dimensions)
{
    globals.window_dimensions.x = new_dimensions.x;
    globals.window_dimensions.y = new_dimensions.y;
    SDL_SetWindowSize(sdlWindow, globals.window_dimensions.x, globals.window_dimensions.y);
    glViewport(0, 0, globals.window_dimensions.x, globals.window_dimensions.y);
}

template <class state_t>
SDL_Window* Application<state_t>::getWindow()
{
    return sdlWindow;
}

template <class state_t>
void Application<state_t>::setCustomState(const state_t& state)
{
    custom_state = state;
}

#define IMPORT_GLOBALS extern Globals globals;

#endif
