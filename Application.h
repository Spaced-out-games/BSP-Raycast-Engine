#pragma once
#define SDL_MAIN_HANDLED


//#define CULL
#include <SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include<chrono>
#ifndef APPLICATION_H
#define APPLICATION_H

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
    float* getDeltaTime() { return deltaTime; };
    SDL_Window* getWindow();
    glm::vec2* getWindowDimensions() { return &windowDimensions; }



    void setWindowDimensions(glm::uvec2 new_dimensions);
    void setCustomState(const state_t& state);

    ImGuiContext* get_gui() const;

    // State management
    state_t& getCustomState();

    // tick / bootstrapping methods
    virtual void bootstrap() { std::cout << "bootstrap\n"; };
    virtual void gl_tick() { std::cout << "tick\n"; };
    virtual void gui_tick(ImGuiContext& gui) {};
    SDL_Event events;
    state_t custom_state;
//private:
    void init_SDL();
    ImGuiContext* imgui_context = nullptr;
    float* deltaTime;// Make these private again and into
    
    glm::vec2 windowDimensions;
    SDL_Window* sdlWindow;
    SDL_GLContext glContext;

    bool vsync_enabled;
    bool running;
    bool init_success;
    bool mFullscreen;


    std::string mTitle;
    void set_deltaTime_ptr(float* ptr) { deltaTime = ptr; }
};



template <class state_t>
Application<state_t>::Application(int argc, char** argv) :
    sdlWindow(nullptr),
    glContext(nullptr),
    windowDimensions(1920.0f, 1080.0f),
    deltaTime(nullptr),
    vsync_enabled(false),
    running(false),
    init_success(false),
    mFullscreen(false),
    mTitle("Engine")

{
    
    //std::vector<std::string> arguments (argv, argv + argc);



    #ifdef _DEBUG
    std::cout << "engineCore constructed\n";
    #endif
    #ifdef _DEBUG
    #endif // DEBUG
    init_SDL();
    //SDL_Rect displayBounds;
    //if (SDL_GetDisplayBounds(0, &displayBounds) != 0) {
    //    std::cerr << "SDL_GetDisplayBounds failed! SDL_Error: " << SDL_GetError() << std::endl;
    //}
    //else {
    //    windowDimensions = glm::vec2(displayBounds.w, displayBounds.h);
    //}
    //SDL_SetWindowSize(sdlWindow, (int)displayBounds.w, (int)displayBounds.h);
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
    // Initialize SDL. In order
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Set OpenGL version (here we use OpenGL 3.3). In order
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Enable multi-sample buffers
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16); // Set number of samples

    // in order
    // Create SDL window
    sdlWindow = SDL_CreateWindow(mTitle.c_str(),
        
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowDimensions.x, windowDimensions.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    #ifdef _DEBUG
    SDL_SetWindowPosition(sdlWindow, 0, 0);
    #endif

    // Create an SDL window. In order
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

    // Initialize GLEW. In order
    glewExperimental = GL_TRUE; // Enable experimental features for core profile
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    // Set the swap interval for the current OpenGL context. Present
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Set the viewport
    glViewport(0, 0, windowDimensions.x, windowDimensions.y);

    // Set the clear color
    glClearColor(0.0f, 0.4f, 0.5f, 1.0f);

    // Create ImGui context and store it in the Application
    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);

    // Set up ImGui IO and style
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Initialize ImGui SDL and OpenGL backends
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
    glDepthFunc(GL_LESS); // Accept fragments with lower depth values
    #ifdef CULL
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back faces
    glFrontFace(GL_CW); // Front face is counter-clockwise
    #endif
    running = true;

    while (running)
    {

        auto start = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tick the engine, passing a pointer to the application
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
        *deltaTime = elapsed.count();
    }
    return 0;
}

template <class state_t>
SDL_GLContext& Application<state_t>::getContext()
{
    return glContext;
}

template <class state_t>
void Application<state_t>::setWindowDimensions(glm::uvec2 new_dimensions)
{
    windowDimensions.x = new_dimensions.x;
    windowDimensions.y = new_dimensions.y;
    SDL_SetWindowSize(sdlWindow, windowDimensions.x, windowDimensions.y);
    glViewport(0, 0, windowDimensions.x, windowDimensions.y);
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




/*
void render(model& m, glm::vec3 color, glm::mat4& transform) {

    glUseProgram(m.shaderProgram);

    GLint colorLoc = glGetUniformLocation(m.shaderProgram, "color");
    glUniform3f(colorLoc, color.r, color.g, color.b);

    GLint transformLoc = glGetUniformLocation(m.shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    m.draw(transform);
}

*/

#endif